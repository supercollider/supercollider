#!/bin/sh
# pack-script for embedded testing.

DATE=`date +%s`
NAME="dyncall-tests-${DATE}"

mkdir -p _work/${NAME}
printf "#!/bin/sh\n" >_work/${NAME}/run.sh
for I in $* ; do
cp $I/$I _work/${NAME}
printf "./$I\n" >>_work/${NAME}/run.sh
printf "%-20s <- %-20s\n" "$I" "$PWD/$I/$I" >>_work/${NAME}/CONTENTS.txt
done
mkdir -p _packed
chmod +x _work/${NAME}/run.sh
tar -cvzf _packed/${NAME}.tar.gz -C _work ${NAME}
printf "finished: tests are in ${PWD}/_packed/${NAME}.tar.gz, execute run-all.sh\n"

