# Contributing to SuperCollider

First and foremost, thank you! We appreciate that you want to contribute to SuperCollider. Your time is valuable, and your contributions mean a lot to us.

#### What does "contributing" mean?

Creating an issue is the simplest form of contributing to a project. But there are many ways to contribute, including the following:

- Updating or correcting documentation
- Fixing an open issue
- Requesting a feature
- Reporting a bug
- Translating the IDE into a language you know (see [Additional resources](#Additional-resources) below)

If you're new to this project, check out the issues tagged ["good first issue"](https://github.com/supercollider/supercollider/issues?q=is%3Aopen+is%3Aissue+label%3A%22good+first+issue%22) – fixing one of these is a great way to get started as a contributor!

If you'd like to learn more about contributing in general, the [Guide to Idiomatic Contributing](https://github.com/jonschlinkert/idiomatic-contributing) has a lot of useful information.

#### Showing support for SuperCollider

Please keep in mind that open source software is built by people like you, who spend their free time creating things the rest the community can use.

Don't have time to contribute? No worries, here are some other ways to show your support for SuperCollider:

- Star the [project](https://github.com/supercollider/supercollider) on Github
- Send a message on the [sc-dev](http://new-supercollider-mailing-lists-forums-use-these.2681727.n2.nabble.com/mailing_list/MailingListOptions.jtp?forum=2681767) mailing list
- Join us on [Slack](https://slackin-apxbmqnfui.now.sh/)
- Join the [Facebook group](https://www.facebook.com/groups/supercollider/)
- Tweet your support for SuperCollider

## Getting Started

- Make sure you have a [GitHub account](https://github.com/signup/free)
- Fork the repository on GitHub

## Issues

### Before creating an issue

Try to follow these guidelines:

- **Investigate the issue**: What is the minimum effort or code required to produce the issue? Does it happen every time? Can you get it to happen on someone else's computer? Someone else's operating system?
- **See if a ticket already exists**: Search SuperCollider's [open issues](https://github.com/supercollider/supercollider/issues). If an issue for your problem already exists, leave your comments in the issue's thread. Make sure you give your version and system info plus any information that you don't see already noted in the ticket.
- **Ask the community**: If you're unsure about how to investigate or recreate an issue, it may help to ask the [sc-dev](http://new-supercollider-mailing-lists-forums-use-these.2681727.n2.nabble.com/mailing_list/MailingListOptions.jtp?forum=2681767) or [sc-users](http://new-supercollider-mailing-lists-forums-use-these.2681727.n2.nabble.com/mailing_list/MailingListOptions.jtp?forum=2676391) mailing lists. You can also try asking on [Slack](https://scsynth.slack.com) or on the [Facebook group](https://www.facebook.com/groups/supercollider/).
- **Proceed in creating your issue**: Be sure to create your issue in the appropriate repository. Note that [sc3-plugins](https://github.com/supercollider/sc3-plugins) are maintained in their own repository separate from SuperCollider.
- **One ticket at a time**: If you have multiple issues to report, please open separate tickets for each one.

### Creating an issue

Please be as descriptive as possible when creating an issue. In order for us to better answer your questions and effectively address your issue, please provide the following information:

- **version**: Note the version of SuperCollider are you using
- **system**: Note your operating system and version
- **extensions, plugins, helpers, etc.** (if applicable): List any extensions or modifications you are using
- **error messages**: Paste all error messages *in their entirety* into the issue, or use a [gist](https://gist.github.com/) if the messages are very long.

When an issue involves a crashing or unresponsive executable and you don't know why, providing a crash report can give developers a very helpful first step toward resolving the problem. See "Generating a crash report" below for instructions for your platform.

## Pull Requests

### Before making changes

- Create a fork of the SuperCollider repository.
- Clone the repo and its submodules locally:

			git clone --recursive https://github.com/your-name/supercollider.git

	- If you have created the fork before, bring it up-to-date with the SuperCollider repository. See [Updating your fork](#Updating-your-fork) below for details.
- Create a topic branch from where you want to base your work.
	- Your topic branch should be based on `develop`, unless it is a trivial bug fix or documentation change, in which case it should be based on the latest release (`3.x`) branch.
	- Our branch naming convention is `topic/branch-description`: for example, `topic/fix-sinosc` or `topic/document-object`.
	- To quickly create a topic branch based on develop: `git checkout -b topic/my-fix develop`.
	- Please do not work off of the `master` branch, which is stable and only includes releases.
- As time passes, make sure to keep your fork updated - see [Updating your fork](#Updating-your-fork) below.

### Making changes

- Make commits of logical units.
- Please refer to [Code Style Guide](https://github.com/supercollider/supercollider/wiki/Code-style-guidelines). Note that code style, such as whitespace conventions, depend on the language (`C++` vs `SuperCollider` vs `SCDoc Markup`)
- Make sure your commit messages are descriptive and in the proper format.
	- Commit messages follow the schema "category: content", e.g. `docs: Make the example in CONTRIBUTING imperative and concrete`, or `help: Update RunningSum2 help file`, or `class library: do this and that`, or `plugins: add missing function definition`.
	- A more complete example:

	        docs: Make the example in CONTRIBUTING imperative and concrete

	        Without this patch applied the example commit message in the CONTRIBUTING
	        document is not a concrete example. This is a problem because the
	        contributor is left to imagine what the commit message should look like
	        based on a description rather than an example. This patch fixes the
	        problem by making the example concrete and imperative.

	        The first line is a real life imperative statement which optionally identifies
	        the component being changed. The body describes the behavior without the patch,
	        why this is a problem, and how the patch fixes the problem when applied.

- Make sure you have added the necessary tests for your changes.
- Make sure you have documented your changes, if necessary.

### Submitting changes as Pull Requests

- Push your changes to a topic branch in your fork of the SuperCollider repository. If you are working locally, do this with `git push -u origin topic/branch-description`. `origin` should be the remote of your fork; check with `git remote -v`.
- Submit a pull request to the SuperCollider repository.
- The core team looks at pull requests on a regular basis in a public meeting that is held on a weekly basis. The meeting times are announced on the sc-dev mailing list.
- You may receive feedback and requests for changes. We expect changes to be made in a timely manner. We may close pull requests if they aren't showing any activity.

### Updating your fork

In order to keep your fork up-to-date, you need to point it to the main SuperCollider repository. This is done by adding the main repository as a remote, usually called `upstream`. **Please note:** naming the main repository `upstream` is just a convention, not a requirement. If you already have a differently named remote pointing to the main SuperCollider repository, you can use that name instead.
- If you haven't yet added the `upstream` remote, you can add it by doing the following:
	- Check the list of remotes: `git remote -v`. The output should look like this:

			origin	https://github.com/your-name/supercollider.git (fetch)
			origin	https://github.com/your-name/supercollider.git (push)

	- Add a new remote called `upstream`, pointing to the SuperCollider repository:

			git remote add upstream https://github.com/supercollider/supercollider.git

	- Check the list of remotes again: `git remote -v`. Now the output should look like this:

			origin	https://github.com/your-name/supercollider.git (fetch)
			origin	https://github.com/your-name/supercollider.git (push)
			upstream	https://github.com/supercollider/supercollider.git (fetch)
			upstream	https://github.com/supercollider/supercollider.git (push)

	- You can now proceed to update your fork.
- If you've already added the `upstream` remote, you can update your fork by doing the following:
	- Be sure to have all local changes committed before proceeding with the update
	- Fetch changes made to the `upstream` repository: `git fetch upstream`
	- Checkout the `develop` branch: `git checkout develop`
	- Pull changes into the `develop` branch: `git pull upstream develop`. Your `develop` branch is now up-to-date.
	- If you've already created your topic branch, you can update it with the changes in `develop` by either rebasing or pulling - see [Notes on rebasing](#Notes-on-rebasing).
	- If you haven't yet created your topic branch, proceed to creating it as described in the [Pull Requests](#Pull-Requests) section.

### Notes on rebasing and merge conflicts

It is almost never a good idea to resolve merge conflicts via the GitHub interface or by merging the main branch in locally. This creates noise in commit history and makes it more difficult to perform other operations on branches later. In the SuperCollider project, the preferred way to resolve merge conflicts in a pull request is by rebasing.

See `git help rebase` for `rebase` usage examples that include graphical representations.

Rebasing (`git help rebase`) is a very useful command for four scenarios in particular:

1. If you'd like to incorporate new commits on the base branch that contain relevant fixes or
   features into your topic branch.
2. If you'd like to resolve merge conflicts.
3. If you'd like to change the branch your topic branch is based on. This may happen if, for instance, a maintainer requests that you make your bug fix merge request against a release branch (e.g. 3.9, 3.10) instead of develop. You will then need to rebase your topic branch onto the appropriate release branch.
4. If you'd like to rewrite your commit history by combining or reordering some commits (not recommended for those newer to git).

Rebase has an interactive mode (`git rebase -i`) which will show exactly which commits will be applied to the new base, and the order in which they will be applied. This can be very helpful when you're not completely sure what the result of a rebase will be.

For scenarios (1) and (2), suppose that the current branch is `topic`, which is based on an old commit from the `develop` branch. To rebase, you can execute `git rebase -i develop`. You can examine the change list to make sure it's correct before continuing. Git will stop if it encounters a merge conflict, and give instructions on how to resolve it and continue the rebase.

For scenario (3), suppose that there are three branches: `develop`, the release branch `3.10`, and the topic branch `topic` which is based on `develop`. Suppose that `topic` is currently checked out.  You would like it to be based on `3.10` instead of `develop`. An easy way to do this is with `git rebase -i --onto 3.10 develop topic`. Beware that you may need to resolve merge conflicts during this rebase.

After rebasing, you may find it helpful to display the pretty-printed commit log with `git log --oneline --graph --decorate` to make sure all is well.

In any case, after rebasing your local branch will now be out of sync with your remote branch. You will have to resolve this by force pushing: `git push --force origin topic`. If you realize later that you made a mistake with your rebase, it's always possible to go back to your previous local state using `git reflog`.

For scenario (4), `git help rebase`'s section "Interactive mode" has extensive documentation on how to reorder and recombine commits. Also refer to the section on `--autosquash` for ideas on how to combine these features into a streamlined rebase-oriented workflow.

## Generating a crash report

### macOS

Crash logs are in `~/Library/Logs/DiagnosticReports`. The following command will list the crash reports for a SuperCollider program with the most recent first:

    ls -lt ~/Library/Logs/DiagnosticReports | grep -E 'SuperCollider|sclang|scsynth|supernova'

If the app is hanging, and you think you know which one it is, you can force it to crash and produce a log by sending it a segfault signal:

    pkill -SIGSEGV <sc-executable> # may need to execute twice to force a crash

The crash log will be placed in `~/Library/Logs/DiagnosticReports` with the others.

### Linux

A core dump file is generated when an application crashes. See [this helpful article](https://linux-audit.com/understand-and-configure-core-dumps-work-on-linux/) for information on core dumps and how to enable them on your machine. You don't need to send us the full core dump (it will probably be quite large), but you can generate a helpful backgrace with gdb:

    gdb <sc-executable> <core-file> -ex where -ex quit

See `man gdb` for more information on using core files.

For a hanging process, you can use the command `pkill -SIGSEGV <sc-executable>` to force a crash, which will then produce a core dump.

### Windows

We don't currently have an easy way to get good crash log information on Windows. You can view logs in Event Viewer, but there's not enough information there that would be helpful for us. If you know of an easy way to get a high-quality crash report on Windows, let us know!

## Additional resources

More information can be found on the [git workflow wiki page](https://github.com/supercollider/supercollider/wiki/git-workflow-and-guidelines).

You can also refer to Github's guide to [forking a repository](https://help.github.com/articles/fork-a-repo/) and to [syncing a fork](https://help.github.com/articles/syncing-a-fork/).

Instructions on adding translation files for the IDE can be found in [DEVELOPING.md](./DEVELOPING.md).

## Above and beyond

Here are some tips for creating idiomatic issues. Taking just a little bit extra time will make your issue easier to read, easier to resolve, more likely to be found by others who have the same or similar issue in the future.

- Read the [Guide to Idiomatic Contributing](https://github.com/jonschlinkert/idiomatic-contributing)
- Take some time to learn basic markdown. This [markdown cheatsheet](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet) is super helpful, as is the GitHub guide to [basic markdown](https://help.github.com/articles/markdown-basics/).
- Learn about [GitHub Flavored Markdown](https://help.github.com/articles/github-flavored-markdown/). And if you want to really go above and beyond, read [mastering markdown](https://guides.github.com/features/mastering-markdown/).
- Use backticks to wrap code. This ensures that code will retain its format, making it much more readable to others
- Use syntax highlighting by adding "supercollider" or the appropriate language name after the first "code fence" ("\`\`\`supercollider")
