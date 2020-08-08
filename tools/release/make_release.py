#!/bin/env python

# This script provides an automated checklist for running through all the steps required for a release.

from collections import deque
import sys
import semantic_version
from semantic_version import Version

import utilities

def check_step(step: str) -> bool:
    print("\nCHECK: ", step)
    response = input("  Press Y OR y to continue; Anything else to Quit: ")
    if response not in ['Y', 'y']:
        return False
    else:
        return True

# Stage base class.
# To make a release, we run through a list of stages in order.
# To undo the work done so far in a release ("bail out"), undo the stages
# completed so far in the reverse order.
class Stage:
    # 'do' should return True to indicate the task succeeded and the release can proceed, or False to indicate the task
    # failed and we should bail out of the release.
    def do(self) -> bool:
        return True

    def undo(self):
        pass

# Performs no actions, simply asks the user whether or not they have completed a task.
class YesNoStage(Stage):
    def __init__(self, prompt):
        self.prompt = prompt

    def do(self) -> bool:
        return check_step(self.prompt)

    def undo(self):
        print("UNDOING:", self.prompt)


class YesNoStageWithUrl(YesNoStage):
    def __init__(self, prompt, url):
        super().__init__(prompt)
        self.url = url

    def do(self) -> bool:
        utilities.open_url(self.url)
        return super().do()


def main(version: Version):
    prompts = [
        "Is the repo clean?",
        "If this is a minor release, have you made the release branch?",
        "Is the repo on the current release branch?",
        ["Have all the discussions in the pinned 'x.y.z discussions' ticket been resolved?", "https://github.com/supercollider/supercollider/issues"],
        ["If this is a patch release, have all the PRs in the cherry-pick GitHub project been added to the release branch?", "https://github.com/supercollider/supercollider/projects/"],
        "Have all the deprecations been either removed or deferred to a later release?\n      Deprecations are removed on a case-by-case basis with each minor (3.x) release.\n      Corresponding UGen and primitive code should also be removed.\n      Be careful when deprecating UGens and be considerate of alternate clients!",
        "Have all the removed deprecations been documented in the changelog?",
        # depends on branch name
        # https://github.com/supercollider/supercollider/blob/3.11/README.md#platform-support
        "Have you reviewed the platform support information in the main README.md for accuracy?",

        "Have you updated SCVersion.txt?",
        "Have you updated CHANGELOG.md with information about merged PRs?",
        "Have you updated CHANGELOG.md with information about platform support changes?",

        "Have you made sure the schelp file 'News in 3.x' is up to date with the changelog by running the conversion script?", # XXX where is the script?
        "Have you made sure HelpSource/Help.schelp points to the latest 'News in 3.x' schelp file?",
        # depends on branch name
        # https://github.com/supercollider/supercollider/blob/develop/CHANGELOG.md#change-log
        "If this is a proper release, have you updated the release history in README.md?",
        "If this is a proper release, have you merged the current release branch into master with git merge --no-ff?",
        "Have you tagged the release?",
        "Did you create the release announcement text?",
        ["Have you created a release on GitHub?", "https://github.com/supercollider/supercollider/releases/new"],
        "Have you run ./package/create_source_tarball.sh -v <version> (where version is the version tag, e.g. Version-3.11.0) to create a source tarball (including submodules)?",
        "Have you optionally run the script with -s <email-or-keyid> (where email-or-keyid is a valid PGP key id of the release manager) to also create a detached PGP signature for the source tarball?",
        ["Have you uploaded source tarball (and optionally detached PGP signature)?", f"https://github.com/supercollider/supercollider/releases/edit/Version-{version}"],
        ["Are builds for macOS, Linux, and Windows uploaded from CI?",f"https://github.com/supercollider/supercollider/releases/tag/Version-{version}"],
        "Have you codesigned and notarized a macOS app bundle?",
        ["Have you uploaded the signed macOS app bundle?", f"https://github.com/supercollider/supercollider/releases/edit/Version-{version}"],
        ["Have you made sure to note known-to-work platform versions and any changes in platform support on the Github release page?", f"https://github.com/supercollider/supercollider/releases/tag/Version-{version}"],
        ["If it is a full release, did you update the website download page?", "https://supercollider.github.io/download"],

        "Did you do the same for sc3-plugins?", # XXX review what does this mean

        ["Did you update the sc3-plugins page?", "https://supercollider.github.io/sc3-plugins/"],
        ["If it's a proper release, did you update the Wikipedia page?", "https://en.wikipedia.org/wiki/SuperCollider"],

        "Have you created the text with an abbreviated changelog for announcing?", # XXX review what does this mean
        ["If this is a proper release, did you announce on GitHub website?", "https://supercollider.github.io/archive"],
        "Did you announce on sc-users mailing list?",
        "Did you announce on sc-dev mailing list?",
        ["Did you announce on scsynth.org?", "https://scsynth.org"],
        # open slack client?
        "Did you announce on Slack #general?",
        # maybe remove? or indicate optional?
        ["Did you announce on Facebook group?", "https://www.facebook.com/groups/2222754532"],
        # maybe remove? or indicate optional?
        "Did you announce on Reddit (/r/supercollider)?",

        # depends on current branch
        # https://github.com/supercollider/supercollider/compare/develop...3.11
        "If it's a beta release, did you merge the current release branch into develop? Do not merge the release branch into master yet!",
        ["If it's a proper release, did you merge master into develop?", "https://github.com/supercollider/supercollider/compare/develop...master"]
            ]

    stack = deque()
    for prompt in prompts:
        if type(prompt) == str:
            stage = YesNoStage(prompt)
        else:
            stage = YesNoStageWithUrl(prompt[0], prompt[1])

        if stage.do():
            stack.append(stage)
        else:
            print("\nUndoing release stages\n")
            while len(stack):
                stack.pop().undo()
            break

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: make_release.py <version>")
        sys.exit(1)

    if not semantic_version.validate(sys.argv[1]):
        print("The supplied version does not comply with SemVer rules. It should be in the form x.y.z[-prerelease]")
        sys.exit(2)

    main(Version(sys.argv[1]))
