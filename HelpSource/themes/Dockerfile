# IF YOU CHANGE ANY VERSIONS HERE ALSO CHANGE THEM IN THE CI/CD STEP
# LOCATED AT .github/workflows/actions.yml

FROM node:lts-alpine3.16

RUN npm instal -g sass@1.54.4

ENTRYPOINT [ "/bin/sh", "/sc_docs/themes/build_css.sh" ]
