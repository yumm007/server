#/bin/bash
svn commit -m "$1"
git commit -a -m "$1"
git pull origin master
git push origin master
