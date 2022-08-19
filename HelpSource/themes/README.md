# CSS themes

In order to have a consistent style between the Qt SC IDE themes and
the HTML docs [PR #5487](https://github.com/supercollider/supercollider/pull/5487)
introduced a switch from `.css` to `.scss` as this allows us to create different themed `.css` files from a single base `.scss` file.
SCSS is a superset of CSS and introduces variables which come
in handy when working with different color styles.

The important thing to keep in mind: do **NOT modify the CSS files directly anymore** as they are automatically generated/compiled out of the SCSS files.

## Building CSS files

As we do not want to add a *SASS* compiler as a build dependency for SuperCollider we add the built CSS files into the git repository itself.

Therefore, after modifications to the `.scss` files you need to build the `.css` files again so they will be used by the IDE or the website.
In order to keep the versions fixed for consistent builds across multiple platforms it is recommended to use [Docker](https://docker.com) to build the `.css` files with which it boils down to two commands - one to set up the build environment and another one to build the actual files.

### Building the Docker image

A Docker image can be regarded as a Linux environment whose setup we can script via a `Dockerfile`.
This allows us to pin versions of software and dependencies, allowing for a deterministic build environment regardless of ones own operating system and installed software.

To build the Docker image run the following command in the `HelpSource/themes` directory (where this `README.md` is located)

```shell
docker build -t sc-doc-themes .
```

### Building CSS files via a Docker container

After building the Docker image we can create an instance from this image (called a *Docker container*) and run the build step with the necessary files.
As the container itself has on default no access to our host machine we will need to tell it to have access the directory where the `.scss` files are located and where the `.css` files shall be (over-)written to.

Assuming you are again in the `HelpSource/themes` directory with a shell the command for a UNIX environment (e.g. Linux and macOS) or a Windows Powershell is

```shell
docker run -v ${PWD}/..:/sc_docs/ sc-doc-themes
```

and for Windows Command Line (cmd)

```cmd
docker run -v %cd%\..:/sc_docs/ sc-doc-themes
```

This command will overwrite any existing `.css` themed files.

### Setting up live SCSS compilation for development

After you have set up the docker image you can also use it to compile any `.scss` modifications directly to `.css` on the fly.

When working e.g. on the *dracula* theme, run

* Unix/Powershell

  ```shell
  docker run -it --init -v ${PWD}/..:/sc_docs/ --entrypoint sass sc-doc-themes --watch /sc_docs/themes/dracula.scss /sc_docs/scdoc.css
  ```

* Windows CMD

  ```cmd
  docker run -it --init -v %cd%\..:/sc_docs/ --entrypoint sass sc-doc-themes --watch /sc_docs/themes/dracula.scss /sc_docs/scdoc.css  
  ```

and use e.g. the [vscode](https://code.visualstudio.com/) [live server](https://marketplace.visualstudio.com/items?itemName=ritwickdey.LiveServer) to view the modifications on e.g. `Browse.html` in your browser.
To stop this simply hit `Ctrl-c`.

### Building in CI/CD

When checking in any built CSS files a CI/CD pipeline will verify if those files are matching when build with the proposed *SASS* environment.

If this pipeline fails

* check if you have build the SCSS files on the proper *SASS* version (check the `Dockerfile`)
* check on [npm](https://www.npmjs.com/package/sass) if the used SASS version was removed/unpublished and bump it if necessary
* Check the [*dart-sass* git repo](https://github.com/sass/dart-sass) for any news

To run the checks locally on your machine set up the Docker image and run

* on Unix or Powershell

  ```shell
  docker run -v ${PWD}/..:/sc_docs/ --entrypoint /bin/sh sc-doc-themes /sc_docs/themes/build_css.sh verify
  ```

* on Windows CMD
  
  ```cmd
  docker run -v %cd%\..:/sc_docs/ --entrypoint /bin/sh sc-doc-themes /sc_docs/themes/build_css.sh verify
  ```

The result should look like

```shell
❯ docker run -v ${PWD}/..:/sc_docs/ --entrypoint /bin/sh sc-doc-themes /sc_docs/themes/build_css.sh verify
Sass version 1.54.4 compiled with dart2js 2.17.6
Verify built files
./classic.css is identical
./dark.css is identical
./default.css is identical
./dracula.css is identical
./solarizedDark.css is identical
./solarizedLight.css is identical
scdoc.css matches default.css
Everything looks fine
```
