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
- Create a topic branch from where you want to base your work.
	- This is usually the `master` branch.
	- Our branch naming convention is `topic/branch-description`: for example, `topic/fix-sinosc` or `topic/document-object`.
	- To quickly create a topic branch based on master: `git checkout -b topic/my-fix master`.
	- Please avoid working directly on the `master` branch.

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

### Submitting changes as pull Requests

- Push your changes to a topic branch in the fork of your repository. If you are working locally, do this with `git push -u my-gh-repo topic/my-fix`. `my-gh-repo` is typically `origin`; check with `git remote -v`.
- Submit a pull request to the SuperCollider repository.
- The core team looks at pull requests on a regular basis in a weekly meeting that we hold in a public meeting. The hangout is announced in the weekly status updates that are sent to the sc-dev list.
- You may receive feedback and requests for changes. We expect changes to be made in a timely manner. We may close the pull request if it isn't showing any activity.

## Above and beyond

Here are some tips for creating idiomatic issues. Taking just a little bit extra time will make your issue easier to read, easier to resolve, more likely to be found by others who have the same or similar issue in the future.

- read the [Guide to Idiomatic Contributing](https://github.com/jonschlinkert/idiomatic-contributing)
- take some time to learn basic markdown. This [markdown cheatsheet](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet) is super helpful, as is the GitHub guide to [basic markdown](https://help.github.com/articles/markdown-basics/).
- Learn about [GitHub Flavored Markdown](https://help.github.com/articles/github-flavored-markdown/). And if you want to really go above and beyond, read [mastering markdown](https://guides.github.com/features/mastering-markdown/).
- use backticks to wrap code. This ensures that code will retain its format, making it much more readable to others
- use syntax highlighting by adding "supercollider" or the appropriate language name after the first "code fence" ("\`\`\`supercollider")
