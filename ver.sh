#!/bin/sh

t=`date`
h=`hostname`
u=${USER:-root}

st=`git status`

git=`git rev-parse --verify --short HEAD 2>/dev/null`

git_b=`git rev-parse --abbrev-ref HEAD`
if [ -n "$git_b" ] ; then
git="${git}(${git_b})"
fi

mgit=`cd mruby;git rev-parse --verify --short HEAD 2>/dev/null`

mgit_b=`git rev-parse --abbrev-ref HEAD`
if [ -n "$mgit_b" ] ; then
mgit="${mgit}(${git_b})"
fi

if git diff-index --name-only HEAD | read dummy; then
git="${git}-dirty"
fi

verinfo="rtlbm-mruby ${git} ${t}\\r\\nmruby ${mgit}\\r\\n"

cat << EOF > ver.c

#define VERSTR	"${verinfo}"

char version[] = VERSTR;
EOF
