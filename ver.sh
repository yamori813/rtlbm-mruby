#!/bin/sh

t=`date`
h=`hostname`
u=${USER:-root}

st=`git status`

git=`git rev-parse --verify --short=7 HEAD 2>/dev/null`

repo=`git remote get-url origin | sed 's/.*\///;s/\..*//'`

git_b=`git rev-parse --abbrev-ref HEAD`
if [ -n "$git_b" ] ; then
git="${git}(${git_b})"
fi

mgit=`cd mruby;git rev-parse --verify --short=7 HEAD 2>/dev/null`

mgit_b=`git rev-parse --abbrev-ref HEAD`
if [ -n "$mgit_b" ] ; then
mgit="${mgit}(${git_b})"
fi

if git diff-index --name-only HEAD | read dummy; then
git="${git}-dirty"
fi

verinfo="${repo} ${git} ${t}\\r\\nmruby ${mgit} - %d.%d.%d\\r\\n"

cat << EOF > ver.c

#define VERSTR	"${verinfo}"

char version[] = VERSTR;
EOF
