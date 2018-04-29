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

if git diff-index --name-only HEAD | read dummy; then
git="${git}-dirty"
fi

verinfo="rtlbm-mruby ${git} ${t} ${u}\\n"

cat << EOF > ver.c

#define VERSTR	"${verinfo}"

char version[] = VERSTR;
EOF
