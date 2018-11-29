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
- **Proceed in creating your issue** Be sure to create your issue in the appropriate repository. Note that [sc3-plugins](https://github.com/supercollider/sc3-plugins) are maintained in their own repository separate from SuperCollider.

### Creating an issue

Please be as descriptive as possible when creating an issue. In order for us to better answer your questions and effectively address your issue, please provide the following information:

- **version**: Note the version of SuperCollider are you using
- **system**: Note your operating system and version
- **extensions, plugins, helpers, etc.** (if applicable): List any extensions or modifications you are using
- **error messages**: Paste all error messages *in their entirety* into the issue, or use a [gist](https://gist.github.com/) if the messages are very long.

## Pull Requests

### Before making changes

- Create a fork of the SuperCollider repository.
- Clone the repo and its submodules locally:

			git clone --recursive https://github.com/your-name/supercollider.git

	- If you have created the fork before, bring it up-to-date with the SuperCollider repository. See [Updating your fork](#Updating-your-fork) below for details.
- Create a topic branch from where you want to base your work.
	- Your topic branch should be based on `develop`.
	- Our branch naming convention is `topic/branch-description`: for example, `topic/fix-sinosc` or `topic/document-object`.
	- To quickly create a topic branch based on develop: `git checkout -b topic/my-fix develop`.
	- Please avoid working directly on the `develop` branch.
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

- Make sure you have added the necessary tests for your changes. All changes to code should include a test which checks for correct functionality, including regression tests for bug fixes. Info on best practice for Unit tests is available at https://github.com/supercollider/supercollider/wiki/Unit-Testing-Guide
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
			upstream	https://github.com/supercollider/supercollider (fetch)
			upstream	https://github.com/supercollider/supercollider (push)

	- You can now proceed to update your fork.
- If you've already added the `upstream` remote, you can update your fork by doing the following:
	- Be sure to have all local changes committed before proceeding with the update
	- Fetch changes made to the `upstream` repository: `git fetch upstream`
	- Checkout the `develop` branch: `git checkout develop`
	- Pull changes into the `develop` branch: `git pull upstream develop`. Your `develop` branch is now up-to-date.
	- If you've already created your topic branch, you can update it with the changes in `develop` by either rebasing or pulling - see [Notes on rebasing](#Notes-on-rebasing).
	- If you haven't yet created your topic branch, proceed to creating it as described in the [Pull Requests](#Pull-Requests) section.

### Notes on rebasing

Rebasing is one way to integrate changes from one branch onto another, in this case `develop` onto your `topic/branch-description`. You don't usually need to `rebase` unless there have been changes to the code you're working on in `upstream/develop`. If you need to rebase, use `git rebase develop topic/branch-description`. Note that if you rebase after already having pushed your branch, it will make your local branch diverge from the remote `origin` copy. In that case you have two options:
- rebase, after which you'll need to force push: `git checkout topic/branch-description` followed by `git push origin -f`
	- This will change the commit history and is not recommended if there is more than one person working on the branch; however, it will create a clean commit history with only your commits in the pull request.
- don't rebase, but instead `git checkout topic/branch-description`, then `git pull`, followed by `git push origin`
	- The result will be that your pull request will include your commits in addition to all other commits to `develop` since your branch was created (without changing the commit history).

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
