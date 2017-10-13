# Contributing to SuperCollider

First and foremost, thank you! We appreciate that you want to contribute to SuperCollider. Your time is valuable, and your contributions mean a lot to us.

**What does "contributing" mean?**

Creating an issue is the simplest form of contributing to a project. But there are many ways to contribute, including the following:

- Updating or correcting documentation
- Feature requests
- Bug reports

If you'd like to learn more about contributing in general, the [Guide to Idiomatic Contributing](https://github.com/jonschlinkert/idiomatic-contributing) has a lot of useful information.

**Showing support for SuperCollider**

Please keep in mind that open source software is built by people like you, who spend their free time creating things the rest the community can use.

Don't have time to contribute? No worries, here are some other ways to show your support for SuperCollider:

- Star the [project](https://github.com/supercollider/supercollider)
- Tweet your support for SuperCollider
- Join the [Facebook group](https://www.facebook.com/groups/supercollider/)
- Send a message on the [sc-dev](http://new-supercollider-mailing-lists-forums-use-these.2681727.n2.nabble.com/mailing_list/MailingListOptions.jtp?forum=2681767) mailing list
- Join the [Slack team](https://slackin-apxbmqnfui.now.sh/)

## Getting Started

- Make sure you have a [GitHub account](https://github.com/signup/free)
- Fork the repository on GitHub

## Issues

### Before creating an issue

Try to follow these guidelines:

- **Investigate the issue**: what is the minimum effort or code required to produce the issue? Can you get it to happen on someone else's computer? Someone else's operating system? Does it happen every time?
- **See if a ticket already exists**: search SuperCollider's [open issues](https://github.com/supercollider/supercollider/issues). If an issue for your problem already exists, let us know by commenting there. Make sure you give your version and system info and any information that you don't see already noted in the ticket.
- Ask the community: if you are unsure about how to investigate or recreate an issue, it may help to ask the [sc-dev](http://new-supercollider-mailing-lists-forums-use-these.2681727.n2.nabble.com/mailing_list/MailingListOptions.jtp?forum=2681767) and [sc-users](http://new-supercollider-mailing-lists-forums-use-these.2681727.n2.nabble.com/mailing_list/MailingListOptions.jtp?forum=2676391) mailing lists, or the [Facebook group](https://www.facebook.com/groups/supercollider/).
- Create the issue in the appropriate repository. Note that [sc3-plugins](https://github.com/supercollider/sc3-plugins) are maintained separately from SuperCollider.

### Creating an issue

Please be as descriptive as possible when creating an issue. Give us the information we need to successfully answer your question or address your issue by answering the following in your issue:

- **version**: please note the version of SuperCollider are you using
- **system**: please note your operating system and version
- **extensions, plugins, helpers, etc.** (if applicable): please list any extensions or modifications you're using
- **error messages**: please paste any error messages, *in their entirety*, into the issue, or a [gist](https://gist.github.com/).

## Pull Requests

### Before making changes

- Create a fork of the SuperCollider repository.
- Clone the repo and its submodules locally: `git clone --recursive https://github.com/your-name/supercollider.git`.
	- If you have created the fork before, bring it up-to-date with the SuperCollider repository. See [updating your fork](#updating-your-fork) below for details.
- Create a topic branch from where you want to base your work.
	- This is the `develop` branch.
	- Our branch naming convention is `topic/branch-description`: for example, `topic/fix-sinosc` or `topic/document-object`.
	- To quickly create a topic branch based on develop: `git checkout -b topic/my-fix develop`.
	- Please avoid working directly on the `develop` branch.
	- Please do not work off of the `master` branch, which is stable and only includes releases.

### Making changes

- Make commits of logical units.
- Check for unnecessary whitespace, and make sure you are following the whitespace conventions of the edited files, before committing.
- Make sure your commit messages are descriptive and in the proper format:

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

- Push your changes to a topic branch in the fork of your repository. If you are working locally, do this with `git push -u my-gh-repo topic/my-fix`. `my-gh-repo` is typically `origin`; check with `git remote -v`.
- Submit a pull request to the SuperCollider repository.
- The core team looks at pull requests on a regular basis in a weekly meeting that we hold in a public meeting. The hangout is announced in the weekly status updates that are sent to the sc-dev list.
- You may receive feedback and requests for changes. We expect changes to be made in a timely manner. We may close the pull request if it isn't showing any activity.

### Updating your fork
In order to keep your fork up-to-date, you need to point it to the main SuperCollider repository. This is done by adding the main repository as another `remote`, usually called `upstream`. **Please note:** naming the main repository `upstream` is just a convention, not a requirement. If you already have a differently named `remote` pointing to the main SuperCollider repository, you can use that name instead of `upstream`.
- If you have not yet created the `upstream` `remote`, create it first:
	- Check the list of remotes: `git remote -v`. The output should look like this:

			origin	https://github.com/your-name/supercollider.git (fetch)
			origin	https://github.com/your-name/supercollider.git (push)

	- Add a new remote called `upstream`, pointing to the SuperCollider repository:
	`git remote add upstream https://github.com/your-name/supercollider.git`
	- Check the list of remotes again: `git remote -v`. Now the output should look like this:

			origin	https://github.com/your-name/supercollider.git (fetch)
			origin	https://github.com/your-name/supercollider.git (push)
			upstream	https://github.com/supercollider/supercollider (fetch)
			upstream	https://github.com/supercollider/supercollider (push)

	- Now you can proceed to updating your fork (see below).
- If you have already created the `upstream` `remote`, update your fork:
	- Be sure to have all local changes committed before doing this!
	- Update from the `upstream` repository: `git fetch upstream`
	- Checkout the `develop` branch: `git checkout develop`
	- Pull changes into the `develop` branch: `git pull upstream develop`
	- If you have already created your topic branch:
		- Update it with the changes in the `develop`: `git rebase develop topic/branch-description`
	- If you have not yet created your topic branch, proceed to creating it as described in the [Pull Requests section](#before-making-changes)

### Additional resources
More information can be found on the [git workflow wiki page](https://github.com/supercollider/supercollider/wiki/git-workflow-and-guidelines).

You can also refer to Github's guide to [forking a repository](https://help.github.com/articles/fork-a-repo/) and [syncing a fork](https://help.github.com/articles/syncing-a-fork/).

## Above and beyond

Here are some tips for creating idiomatic issues. Taking just a little bit extra time will make your issue easier to read, easier to resolve, more likely to be found by others who have the same or similar issue in the future.

- read the [Guide to Idiomatic Contributing](https://github.com/jonschlinkert/idiomatic-contributing)
- take some time to learn basic markdown. This [markdown cheatsheet](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet) is super helpful, as is the GitHub guide to [basic markdown](https://help.github.com/articles/markdown-basics/).
- Learn about [GitHub Flavored Markdown](https://help.github.com/articles/github-flavored-markdown/). And if you want to really go above and beyond, read [mastering markdown](https://guides.github.com/features/mastering-markdown/).
- use backticks to wrap code. This ensures that code will retain its format, making it much more readable to others
- use syntax highlighting by adding "supercollider" or the appropriate language name after the first "code fence" ("\`\`\`supercollider")
