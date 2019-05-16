#!/usr/bin/env python
from __future__ import print_function, absolute_import, unicode_literals

import difflib
import glob
import os
import re
import string
import subprocess
import sys
import threading
from argparse import ArgumentParser

# Whichcraft backported shutil.which implementation
# Taken from https://github.com/pydanny/whichcraft/blob/master/whichcraft.py (version 0.5.3)
#
# BEGIN BSD-LICENSED CODE
#
# Copyright (c) 2015-2016, Daniel Roy Greenfeld All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted
# provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this list of conditions and
# the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
# and the following disclaimer in the documentation and/or other materials provided with the
# distribution.
#
# * Neither the name of whichcraft nor the names of its contributors may be used to endorse or promote
# products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

try:  # Forced testing
    from shutil import which
except ImportError:  # Forced testing
    # Versions prior to Python 3.3 don't have shutil.which

    def which(cmd, mode=os.F_OK | os.X_OK, path=None):
        """Given a command, mode, and a PATH string, return the path which
        conforms to the given mode on the PATH, or None if there is no such
        file.
        `mode` defaults to os.F_OK | os.X_OK. `path` defaults to the result
        of os.environ.get("PATH"), or can be overridden with a custom search
        path.
        Note: This function was backported from the Python 3 source code.
        """
        # Check that a given file can be accessed with the correct mode.
        # Additionally check that `file` is not a directory, as on Windows
        # directories pass the os.access check.

        def _access_check(fn, mode):
            return os.path.exists(fn) and os.access(fn, mode) and not os.path.isdir(fn)

        # If we're given a path with a directory part, look it up directly
        # rather than referring to PATH directories. This includes checking
        # relative to the current directory, e.g. ./script
        if os.path.dirname(cmd):
            if _access_check(cmd, mode):
                return cmd

            return None

        if path is None:
            path = os.environ.get("PATH", os.defpath)
        if not path:
            return None

        path = path.split(os.pathsep)

        if sys.platform == "win32":
            # The current directory takes precedence on Windows.
            if os.curdir not in path:
                path.insert(0, os.curdir)

            # PATHEXT is necessary to check on Windows.
            pathext = os.environ.get("PATHEXT", "").split(os.pathsep)
            # See if the given file matches any of the expected path
            # extensions. This will allow us to short circuit when given
            # "python.exe". If it does match, only test that one, otherwise we
            # have to try others.
            if any(cmd.lower().endswith(ext.lower()) for ext in pathext):
                files = [cmd]
            else:
                files = [cmd + ext for ext in pathext]
        else:
            # On other platforms you don't have things like PATHEXT to tell you
            # what file suffixes are executable, so just pass on cmd as-is.
            files = [cmd]

        seen = set()
        for dir in path:
            normdir = os.path.normcase(dir)
            if normdir not in seen:
                seen.add(normdir)
                for thefile in files:
                    name = os.path.join(dir, thefile)
                    if _access_check(name, mode):
                        return name

        return None

##############################################################################
# END BSD-LICENSED CODE
##############################################################################

##############################################################################
#
# Constants
#

CLANG_FORMAT_ACCEPTED_VERSION = "8.0.0"

# all the extensions we format with clang-format in SC (no JS!)
CLANG_FORMAT_FILES_REGEX = re.compile('\\.(cpp|hpp|h|c|m|mm)$')

# autogen'd files, don't touch
AUTOGEN_FILES_REGEX = re.compile('(SCDoc\\.tab\\..pp|lex\\.scdoc\\.cpp|lang11d_tab\\..*)$')

# the destination filename for a git diff
DIFF_FILENAME_REGEX = re.compile('^\\+\\+\\+ b/(.*)$', re.MULTILINE)

##############################################################################

def callo(args):
    """Call a program, and capture its output
    """
    return subprocess.check_output(args).decode('utf-8')

def callo_with_input(args, inputdata):
    """Call a program, pipe input into it, and capture its output
    """
    pipe = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    return pipe.communicate(inputdata.encode('utf-8'))[0].decode('utf-8')

def get_base_dir():
    """Get the base directory for repo.
    """
    try:
        return subprocess.check_output(['git', 'rev-parse', '--show-toplevel']).rstrip().decode('utf-8')
    except:
        print("This script must be running in a git repo")
        sys.exit(2)

class Repo(object):
    """Class encapsulates all knowledge about a git repository, and its metadata
        to run clang-format.
    """
    def __init__(self, path):
        self.path = path

    def _callgito(self, args):
        """Call git for this repository, and return the captured output
        """
        # These two flags are the equivalent of -C in newer versions of Git
        # but we use these to support versions pre 1.8.5 but it depends on the command
        # and what the current directory is
        return callo(['git', '--git-dir', os.path.join(self.path, ".git"),
                            '--work-tree', self.path] + args)

    def _callgit(self, args, stdout=None):
        """Call git for this repository without capturing output
        This is designed to be used when git returns non-zero exit codes.
        """
        # These two flags are the equivalent of -C in newer versions of Git
        # but we use these to support versions pre 1.8.5 but it depends on the command
        # and what the current directory is
        return subprocess.call(['git', '--git-dir', os.path.join(self.path, ".git"),
                                '--work-tree', self.path] + args, stdout=stdout)

    def is_detached(self):
        # symbolic-ref returns 1 if the repo is in a detached HEAD state
        with open(os.devnull, 'w') as DEVNULL:
            return self._callgit(["symbolic-ref", "--quiet", "HEAD"], stdout=DEVNULL)

    def is_ancestor(self, parent, child):
        # merge base returns 0 if parent is an ancestor of child
        return not self._callgit(["merge-base", "--is-ancestor", parent, child])

    def is_commit(self, sha1):
        # cat-file -e returns 0 if it is a valid hash
        return not self._callgit(["cat-file", "-e", "%s^{commit}" % sha1])

    def is_working_tree_dirty(self):
        # diff returns 1 if the working tree has local changes
        return self._callgit(["diff", "--quiet"])

    def does_branch_exist(self, branch):
        # rev-parse returns 0 if the branch exists
        return not self._callgit(["rev-parse", "--verify", "--quiet", branch])

    def get_merge_base(self, commit):
        return self._callgito(["merge-base", "HEAD", commit]).rstrip()

    def get_branch_name(self):
        """Get the current branch name, short form
           This returns "master", not "refs/head/master"
           Will not work if the current branch is detached
        """
        branch = self.rev_parse(["--abbrev-ref", "HEAD"])
        if branch == "HEAD":
            raise ValueError("Branch is currently detached")

        return branch

    def add(self, command):       return self._callgito(["add"] + command)
    def checkout(self, command):  return self._callgito(["checkout"] + command)
    def commit(self, command):    return self._callgito(["commit"] + command)
    def diff(self, command):      return self._callgito(["diff"] + command)
    def log(self, command):       return self._callgito(["log"] + command)
    def rev_parse(self, command): return self._callgito(["rev-parse"] + command).rstrip()
    def rm(self, command):        return self._callgito(["rm"] + command)
    def show(self, command):      return self._callgito(["show"] + command)
    def lsfiles(self):            return self._callgito(["ls-files"])

class ClangFormat(object):
    """Class encapsulates finding a suitable copy of clang-format,
    and linting/formating an individual file
    """
    def __init__(self, cf_cmd):
        self.cf_cmd = cf_cmd
        if which(cf_cmd) is None:
            raise ValueError("Could not find clang-format at %s" % cf_cmd)
        self._validate_version()

    def _validate_version(self):
        cf_version = callo([self.cf_cmd, "--version"])

        if CLANG_FORMAT_ACCEPTED_VERSION in cf_version:
            return

        # TODO add instructions to check docs when docs are written
        raise ValueError("clang-format found, but incorrect version at " +
                self.cf_cmd + " with version: " + cf_version + "\nAccepted version: " +
                CLANG_FORMAT_ACCEPTED_VERSION)
        sys.exit(5)

    def lint(self, file_name, print_diff):
        """Check the specified file has the correct format
        """
        with open(file_name, 'rb') as original_text:
            original_file = original_text.read().decode('utf-8')

        # Get formatted file as clang-format would format the file
        formatted_file = callo([self.cf_cmd, '-style=file', file_name])

        if original_file != formatted_file:
            if print_diff:
                original_lines = original_file.splitlines()
                formatted_lines = formatted_file.splitlines()
                result = difflib.unified_diff(original_lines, formatted_lines, file_name, file_name)
                for line in result:
                    print(line.rstrip())

            return False

        return True

    def format(self, file_name):
        """Update the format of the specified file
        """
        if self.lint(file_name, print_diff=False):
            return True

        # Update the file with clang-format
        formatted = not subprocess.call([self.cf_cmd, '-style=file', '-i', file_name])

        # Version 3.8 generates files like foo.cpp~RF83372177.TMP when it formats foo.cpp
        # on Windows, we must clean these up
        if sys.platform == "win32":
            glob_pattern = file_name + "*.TMP"
            for fglob in glob.glob(glob_pattern):
                os.unlink(fglob)

        return formatted

def get_list_from_lines(lines):
    """"Convert a string containing a series of lines into a list of strings
    """
    return [line.rstrip() for line in lines.splitlines()]

def validate_repo_state_for_rebase(commit_before_reformat, commit_after_reformat, target_branch):
    if sys.version_info[0] == 2:
        cwd = os.getcwdu()
    else:
        cwd = os.getcwd()

    if os.path.normpath(cwd) != os.path.normpath(get_base_dir()):
        raise ValueError("reformat-branch must be run from the repo root")

    repo = Repo(get_base_dir())

    if not repo.is_commit(commit_before_reformat):
        raise ValueError("Commit before reformat '%s' is not a valid commit in this repo" %
                commit_before_reformat)

    if not repo.is_commit(commit_after_reformat):
        raise ValueError("Commit after reformat '%s' is not a valid commit in this repo" %
                commit_after_reformat)

    if not repo.is_ancestor(commit_before_reformat, commit_after_reformat):
        raise ValueError(("Commit before reformat '%s' is not a valid ancestor of commit after" +
                " reformat '%s' in this repo") % (commit_before_reformat, commit_after_reformat))

    if repo.is_detached():
        raise ValueError("You must not run this script in a detached HEAD state")

    if repo.is_working_tree_dirty():
        raise ValueError("Your working tree has pending changes. You must have a clean working" +
            " tree before proceeding.\n\nRun `git status` to see your pending changes, and then" +
            " try `git stash save`, `git reset --hard`, `git submodule update` and/or committing" +
            " your changes.")

    merge_base = repo.get_merge_base(commit_before_reformat)

    if not merge_base == repo.rev_parse([commit_before_reformat]):
        raise ValueError(("Merge base is '%s'. Please rebase to '%s' and resolve all conflicts" +
            " before running this script.\n\nTo interactively rebase, use `git rebase -i %s`") %
            (merge_base, commit_before_reformat, commit_before_reformat))

    # We assume the target branch is master, it could be a different branch if needed for testing
    merge_base = repo.get_merge_base(target_branch)

    if not merge_base == repo.rev_parse([commit_before_reformat]):
        raise ValueError("This branch appears to already have advanced too far through the merge process")

    return repo

def get_branch_names(repo):
    # Everything looks good so lets start going through all the commits
    branch_name = repo.get_branch_name()
    new_branch = branch_name + "-reformatted"

    if repo.does_branch_exist(new_branch):
        raise ValueError("The branch '%s' already exists. Please delete the branch '%s', or rename the current branch." % (new_branch, new_branch))

    return (branch_name, new_branch)

def is_3rd_party_file(name):
    return name.find('external_libraries') != -1
def is_autogen_file(name):
    return AUTOGEN_FILES_REGEX.search(name)
def is_clang_formattable(name):
    return CLANG_FORMAT_FILES_REGEX.search(name)

def is_wanted_clang_formattable_file(f):
    """Is this something we want to use ClangFormat to format?
    """
    return is_clang_formattable(f) and not is_3rd_party_file(f) and not is_autogen_file(f)

def get_all_clang_formattable_files(repo):
    files = get_list_from_lines(repo.lsfiles())
    return [f for f in files if is_wanted_clang_formattable_file(f)]

def rebase_branch(clang_format, commit_before_reformat, commit_after_reformat, target_branch):
    """Reformat a branch made before a clang-format run
    """
    clang_format = ClangFormat(clang_format)
    repo = validate_repo_state_for_rebase(commit_before_reformat, commit_after_reformat, target_branch)
    old_branch, new_branch = get_branch_names(repo)
    commits = get_list_from_lines(repo.log(["--reverse", "--pretty=format:%H", "%s..HEAD" % commit_before_reformat]))
    previous_commit_base = commit_after_reformat

    # Go through all the commits the user made on the local branch and migrate to a new branch
    # that is based on post_reformat commits instead
    for idx, commit_hash in enumerate(commits):
        print("--- Formatting " + commit_hash + (" (%s of %s)" % (idx + 1, len(commits))))
        repo.checkout(["--quiet", "--detach", commit_hash])

        deleted_files = []

        # Format each of the files by checking out just a single commit from the user's branch
        commit_files = get_list_from_lines(repo.diff(["HEAD~", "--name-only"]))

        for commit_file in commit_files:

            # Format each file needed if it was not deleted
            if not os.path.exists(commit_file):
                print("\tSkipping file '%s' since it has been deleted in commit '%s'" % (
                        commit_file, commit_hash))
                deleted_files.append(commit_file)
                continue

            if is_3rd_party_file(commit_file):
                print("\tSkipping external libraries file '%s'" % commit_file)
            elif is_autogen_file(commit_file):
                print("\tSkipping autogenerated file '%s'" % commit_file)
            elif is_clang_formattable(commit_file):
                clang_format.format(commit_file)
            else:
                print("\tSkipping file '%s' (no formatting to apply)" % commit_file)

        # Check if anything needed reformatting, and if so amend the commit
        if not repo.is_working_tree_dirty():
            print ("Commit %s needed no reformatting" % commit_hash)
        else:
            repo.commit(["--all", "--amend", "--no-edit"])

        # Rebase our new commit on top the post-reformat commit
        previous_commit = repo.rev_parse(["HEAD"])

        # Checkout the new branch with the reformatted commits
        # Note: we will not name as a branch until we are done with all commits on the local branch
        repo.checkout(["--quiet", "--detach", previous_commit_base])

        # Copy each file from the reformatted commit on top of the post reformat
        diff_files = get_list_from_lines(repo.diff(["%s~..%s" % (previous_commit, previous_commit),
            "--name-only"]))

        for diff_file in diff_files:
            # If the file was deleted in the commit we are reformatting, we need to delete it again
            if diff_file in deleted_files:
                repo.rm([diff_file])
                continue

            # The file has been added or modified, continue as normal
            file_contents = repo.show(["%s:%s" % (previous_commit, diff_file)])

            root_dir = os.path.dirname(diff_file)
            if root_dir and not os.path.exists(root_dir):
                os.makedirs(root_dir)

            with open(diff_file, "w+") as new_file:
                new_file.write(file_contents)

            repo.add([diff_file])

        # Create a new commit onto clang-formatted branch
        repo.commit(["--reuse-message=%s" % previous_commit])

        previous_commit_base = repo.rev_parse(["HEAD"])

    # Create a new branch to mark the hashes we have been using
    repo.checkout(["-b", new_branch])

    print("reformat-branch is done running.\n")
    print("A copy of your branch has been made named '%s', and formatted with clang-format.\n" % new_branch)
    print("The original branch has been left unchanged.")
    print("If you have not just done so, the next step is to rebase the new branch on '%s'.\n" % target_branch)
    print("To undo this, run `git checkout %s && git branch -D %s`" % (old_branch, new_branch))

def is_wanted_diff(diff_text):
    # Extract file name
    match = DIFF_FILENAME_REGEX.search(diff_text)
    if not match:
        raise ValueError("Could not extract filename from diff")
    return is_wanted_clang_formattable_file(match.group(1))

def filter_unwanted_files_from_diff(diff_text):
    # git diff was called with -U0 so all actual diffed lines can't start with '^diff'
    # Couldn't find a way to split on lookaheads, so went with this instead.
    # [1:] to discard initial empty string
    diffs = ['diff' + match for match in re.split('^diff', diff_text, flags=re.MULTILINE)][1:]
    filter_diffs = [diff for diff in diffs if is_wanted_diff(diff)]
    return ''.join(filter_diffs)

def prepare_diff_for_lint_format(clang_format, commit):
    ClangFormat(clang_format) # validation

    repo = Repo(get_base_dir())
    if not repo.is_commit(commit):
        raise ValueError("Commit before reformat '%s' is not a valid commit in this repo" % commit)

    os.chdir(repo.path)

    diff_text = repo.diff([commit, '-U0', '--no-color'])
    return filter_unwanted_files_from_diff(diff_text)

def do_lint(clang_format, clang_format_diff, commit):
    diff_text = prepare_diff_for_lint_format(clang_format, commit)
    lint_out = callo_with_input(['python', clang_format_diff, '-p1', '-binary', clang_format], diff_text)
    print(lint_out, end='')
    if lint_out != '\n':
        sys.exit(1)

def do_format(clang_format, clang_format_diff, commit):
    diff_text = prepare_diff_for_lint_format(clang_format, commit)
    callo_with_input(['python', clang_format_diff, '-i', '-p1', '-binary', clang_format], diff_text)

def do_lintall(clang_format):
    repo = Repo(get_base_dir())
    os.chdir(repo.path)
    clang_format = ClangFormat(clang_format)
    no_changes_needed = True
    for f in get_all_clang_formattable_files(repo):
        no_changes_needed = clang_format.lint(f, True) and no_changes_needed
    if not no_changes_needed:
        sys.exit(1)

def do_formatall(clang_format):
    repo = Repo(get_base_dir())
    os.chdir(repo.path)
    clang_format = ClangFormat(clang_format)
    for f in get_all_clang_formattable_files(repo):
        clang_format.format(f)

def main():
    parser = ArgumentParser(
            usage='''
    format.py lint [commit]
    format.py format [commit]
    format.py lintall
    format.py formatall
    format.py rebase -b base-branch
    format.py rebase commit1 commit2 target

PLEASE READ.

This script provides commands for linting and formatting your working directory. It provides five
commands:
1. `lint` lints the diff between the working directory and a given commit
2. `format` will apply formatting rules to the diff between working directory and given commit
3. `lintall` lints all available files for various formatting rules and indicates any problems.
4. `formatall` formats all available files.
5. `rebase` reformats a branch past the great reformatting wall. It can be run two ways; the second
   is simpler and usually works.
    a. `format.py rebase commit-right-before-reformat commit-after-reformat original-branch`
    b. `format.py 3.10 # or develop`

Rebase requires:
- you have a clean working directory
- you have rebased your branch on commit-right-before-reformat (implicitly for the second usage)
- you have the branch you want to rebase currently checked out

If there is an issue, this script will most likely detect it and provide you with commands to
proceed.

'commit' arguments can be a branch name, tag, or commit hash.

This script will exit with 0 on success, 1 to indicate lint failure, and >1 if some other error
occurs.
''')
    parser.add_argument("-c", "--clang-format", dest="clang_format", default='clang-format',
            help='Command to use for clang-format; will also be passed to clang-format-diff.py')
    parser.add_argument("-b", "--base", dest="base_branch", help='Tries to rebase on the tip of this'
            + ' branch given a base branch name (experimental). This should be the main branch the'
            + ' current branch is based on (3.10 or develop)')
    parser.add_argument("-d", "--clang-format-diff", dest="clang_format_diff", default='clang-format-diff.py',
            help='Command to use for clang-format-diff.py script')
    parser.add_argument("command", help="command; one of lint, format, lintall, formatall, rebase")
    parser.add_argument("commit1", help="for lint and format: commit to compare against (default: HEAD);" +
            " for rebase: commit immediately prior to reformat", nargs='?', default='')
    parser.add_argument("commit2", help="commit after reformat", nargs='?', default='')
    parser.add_argument("target", help="target branch name (likely 3.10 or develop)", nargs='?', default='')

    options = parser.parse_args()

    try:
        if options.command == 'lint' or options.command == 'format':
            commit = 'HEAD' if options.commit1 == '' else options.commit1

            # For portability, we use the full path of the clang-format-diff.py script. subprocess
            # module on Windows won't be able to find a Python-executable python script in PATH, and
            # if we invoke it with `python <script> <args>` then the python interpreter needs the
            # full path of the script. Of course, the downside is that we use whatever `python`
            # resolves to in the host system's shell.
            clang_format_diff_path = which(options.clang_format_diff)
            if clang_format_diff_path is None:
                if options.clang_format_diff == 'clang-format-diff.py':
                    raise ValueError(
                            "Could not find clang-format-diff.py. "
                            "Please ensure that clang %s is installed and that "
                            "clang-format-diff.py is in your PATH."
                            % CLANG_FORMAT_ACCEPTED_VERSION)
                else:
                    raise ValueError("Could not find clang-format-diff.py at %s." % options.clang_format_diff)
            if options.command == 'lint':
                do_lint(options.clang_format, clang_format_diff_path, commit)
            else:
                do_format(options.clang_format, clang_format_diff_path, commit)
        elif options.command == 'lintall':
            do_lintall(options.clang_format)
        elif options.command == 'formatall':
            do_formatall(options.clang_format)
        elif options.command == 'rebase':
            if not options.commit1 or not options.commit2 or not options.target:
                if not options.base_branch:
                    parser.print_help()
                    sys.exit(2)

                if options.base_branch == '3.10':
                    options.commit1 = 'tag-clang-format-3.10^'
                    options.commit2 = options.target = 'tag-clang-format-3.10'
                elif options.base_branch == 'develop':
                    options.commit1 = 'tag-clang-format-develop^'
                    options.commit2 = options.target = 'tag-clang-format-develop'
                else:
                    print("Don't know how to use this base branch: %s. Try using the three-argument " +
                            "version of rebase command")
                    sys.exit(3)

            rebase_branch(options.clang_format, options.commit1, options.commit2, options.target)
        else:
            parser.print_help()
            sys.exit(4)
    except ValueError as ve:
        # print entire traceback to aid in diagnosing issues
        import traceback
        traceback.print_tb(sys.exc_info()[2])
        print("\n*** ERROR:\n" + str(ve) + "\n")
        sys.exit(6)

if __name__ == "__main__":
    main()
