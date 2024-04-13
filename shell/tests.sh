#!/bin/bash

# script to run on all student submissions. You can cd into the
# student's directory and run '../tests.sh"
# To run it on all student submissions, use:
# for i in `find . -maxdepth 1 -mindepth 1 -type d` ; do
#      cd $i ; ../tests.sh ; cd .. ;
# done
# 

GRADEFILE=tests.dat
if [ -f $GRADEFILE ] ; then
    YN="N"
    printf "$GRADEFILE already exists.  Delete and start over? :"
    read YN
    if [ "$YN" == "y" ] ; then YN="Y" ; fi
    # Don't delete anything unless they actually hit "y" or "Y"
    if [ "$YN" == "Y" ] ; then
	rm -f $GRADEFILE
    else
	echo "Resuming"
    fi
else
    touch $GRADEFILE
fi

# Function to add a new line to the grade file
addgrade () {
    printf "correct? "
    read YN
    if [ $YN == "N" -o $YN == "n" ] ; then
	YN="N"
    else
	YN="Y"
    fi
    # add 3 pts for every correct answer
    if [ $YN == "Y" ] ; then
	printf "3\t$1\n" >> $GRADEFILE
    else
	printf "0\t$1\n" >> $GRADEFILE
    fi
}

# clean up if last run was aborted
rm -f /tmp/test1
rm -f /tmp/test1.ans
rm -f /tmp/test1.out

# BEGIN TESTING - Each test adds one line to the grade file. For each
#  test, Check length of grade file and skip tests that are already
#  done.

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 1 ] ; then
    # blank line and exit
    echo TEST 1
    printf "\nexit\n" | ./mish > /tmp/test1
    cat /tmp/test1
    addgrade 'blank line and exit'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 2 ] ; then
    # simple command and exit
    echo TEST 2
    printf "ls -al\nexit\n" | ./mish > /tmp/test1
    ls -al > /tmp/test1.ans
    diff /tmp/test1 /tmp/test1.ans
    addgrade 'simple command and exit'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 3 ] ; then
    # simple command with output redirection and exit
    echo TEST 3
    printf "ls -al > /tmp/test1\nexit\n" | ./mish 
    ls -al > /tmp/test1.ans
    diff /tmp/test1 /tmp/test1.ans
    addgrade 'simple command with output redirection and exit'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 4 ] ; then
    # simple command with input redirection and exit
    echo TEST 4
    ls -al > /tmp/test1.ans
    printf "shuf < /tmp/test1.ans\nexit\n" | ./mish > /tmp/test1.out
    sort -k 9 < /tmp/test1.out > /tmp/test1
    diff /tmp/test1 /tmp/test1.ans
    addgrade 'simple command with input redirection and exit'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 5 ] ; then
    # simple command with input and output redirection and exit
    echo TEST 5
    ls -al > /tmp/test1.ans
    printf "shuf < /tmp/test1.ans > /tmp/test1 \nexit\n" | ./mish 
    sort -k 9 < /tmp/test1 > /tmp/test1.out
    diff /tmp/test1.out /tmp/test1.ans
    addgrade 'simple command with input and output redirection and exit'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 6 ] ; then
    # simple command with output and input redirection and exit
    echo TEST 6
    ls -al > /tmp/test1.ans
    printf "shuf  > /tmp/test1 < /tmp/test1.ans \nexit\n" | ./mish 
    sort -k 9 < /tmp/test1 > /tmp/test1.out
    diff /tmp/test1.out /tmp/test1.ans
    addgrade 'simple command with output and input redirection and exit'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 7 ] ; then
    # single pipe
    echo TEST 7
    ls -al | sort -k 9 > /tmp/test1.ans
    printf "ls -al | sort -k 9" | ./mish > /tmp/test1.out
    diff /tmp/test1.out /tmp/test1.ans
    addgrade 'single pipe'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 8 ] ; then
    # single pipe with output redirect
    echo TEST 8
    ls -al | sort -k 9 > /tmp/test1.ans
    printf "ls -al | sort -k 9  > /tmp/test1.out" | ./mish
    diff /tmp/test1.out /tmp/test1.ans
    echo
    addgrade 'single pipe with output redirect'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 9 ] ; then
    # single pipe with input and output redirect
    echo TEST 9
    ls -al | sort -k 9 > /tmp/test1.ans
    ls -al | shuf > /tmp/test1
    printf "cat </tmp/test1 | sort -k 9 > /tmp/test1.out" | ./mish
    diff /tmp/test1.out /tmp/test1.ans
    echo
    addgrade 'single pipe with input and output redirect'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 10 ] ; then
    # double pipe
    echo TEST 10
    printf "ls -al | shuf | sort -k 9 \nexit\n" | ./mish > /tmp/test1
    ls -al --color=never > /tmp/test1.ans
    diff /tmp/test1 /tmp/test1.ans
    addgrade 'double pipe'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 11 ] ; then
    # double pipe with output redirect
    echo TEST 11
    printf "ls -al | shuf | sort -k 9 > /tmp/test1\nexit\n" | ./mish 
    ls -al --color=never > /tmp/test1.ans
    diff /tmp/test1 /tmp/test1.ans
    addgrade 'double pipe with output redirect'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 12 ] ; then
    # double pipe with input redirect
    echo TEST 12
    ls -al > /tmp/test1.ans
    printf "cat < /tmp/test1.ans| shuf | sort -k 9\nexit\n" | ./mish >/tmp/test1
    diff /tmp/test1 /tmp/test1.ans
    addgrade 'double pipe with input redirect'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 13 ] ; then
    # double pipe with input and output redirect
    echo TEST 13
    ls -al > /tmp/test1.ans
    printf "cat < /tmp/test1.ans| shuf | sort -k 9 > /tmp/test1 \nexit\n" | ./mish 
    diff /tmp/test1 /tmp/test1.ans
    addgrade 'double pipe with input and output redirect'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 14 ] ; then
    # wait on child
    echo TEST 14
    echo Wait for child
    printf "emacs\nls\exit\n" | ./mish 
    addgrade 'wait on child'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 15 ] ; then
    # do not wait on child
    echo TEST 15
    echo Do not wait for child
    printf "emacs & \nls\nexit\n" | ./mish
    addgrade 'do not wait on child'
    echo	    
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 16 ] ; then
    # set PATH 
    echo TEST 16
    echo Unsetting PATH
    printf "PATH=\n/usr/bin/env\nexit\n" | ./mish | grep PATH | grep -v ROOT | grep -v INFO |grep -v GMP|grep -v SDK | grep -v LV2 | grep -v VBOX | grep -v MAN
    echo Setting PATH
    printf "PATH=/bin:/usr/bin\nenv\nexit\n" | ./mish | grep PATH | grep -v ROOT | grep -v INFO |grep -v GMP|grep -v SDK | grep -v LV2 | grep -v VBOX | grep -v MAN
    echo
    echo Unsetting PATH
    printf "PATH=\n/usr/bin/env\nexit\n" | ./mish | grep PATH | grep -v ROOT | grep -v INFO |grep -v GMP|grep -v SDK | grep -v LV2 | grep -v VBOX | grep -v MAN
    echo Setting PATH
    printf "PATH=/bin:/usr/bin\nenv\nexit\n" | ./mish | grep PATH | grep -v ROOT | grep -v INFO |grep -v GMP|grep -v SDK | grep -v LV2 | grep -v VBOX | grep -v MAN
    addgrade 'set PATH'
    echo
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 17 ] ; then
    # set other variable
    echo TEST 17
    echo Setting TESTVAR
    printf "TESTVAR=/usr/bin/emacs\nenv\nexit\n" | ./mish | grep TESTVAR
    addgrade 'set other variable'
    echo
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 18 ] ; then
    # basic script
    echo TEST 18
    echo "ls -al > /tmp/test1.out" > /tmp/test1.sh
    ./mish /tmp/test1.sh
    ls -al > /tmp/test1.ans
    diff /tmp/test1.out /tmp/test1.ans
    addgrade 'basic script'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
    rm -f /tmp/test1.sh
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 19 ] ; then
    # multi-line basic script
    echo TEST 19
    echo "ls -al > /tmp/test1.out" > /tmp/test1.sh
    echo "shuf /tmp/test1.out > /tmp/test1" >> /tmp/test1.sh
    echo "sort -k 9 /tmp/test1 > /tmp/test1.out" >> /tmp/test1.sh
    ./mish /tmp/test1.sh
    ls -al > /tmp/test1.ans
    diff /tmp/test1.out /tmp/test1.ans
    addgrade 'multi-line basic script'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
    rm -f /tmp/test1.sh
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 20 ] ; then
    # multi-line complex script
    echo TEST 20
    echo "ls -al > /tmp/test1" > /tmp/test1.sh
    echo "shuf /tmp/test1 | sort -k 9 > /tmp/test1.out" >> /tmp/test1.sh
    ./mish /tmp/test1.sh
    ls -al > /tmp/test1.ans
    diff /tmp/test1.out /tmp/test1.ans
    addgrade 'multi-line complex script'
    echo
    rm -f /tmp/test1
    rm -f /tmp/test1.ans
    rm -f /tmp/test1.out
    rm -f /tmp/test1.sh
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 21 ] ; then
    # bad syntax: multiple output redirect
    echo TEST 21
    printf "cat ../tests.sh > /tmp/t18a > /tmp/t18b\nexit\n" | ./mish
    addgrade 'bad syntax: multiple output redirect'
    echo
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 22 ] ; then
    # bad syntax: multiple input redirect
    echo TEST 22
    printf "cat < ../tests.sh < ../tests.sh\nexit\n" | ./mish
    addgrade 'bad syntax: multiple input redirect'
    echo
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 23 ] ; then
    # bad syntax: two pipes with no command between
    echo TEST 23
    printf "cat < ../tests.sh || sort \nexit\n" | ./mish
    addgrade 'bad syntax: two pipes with no command between'
    echo
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 24 ] ; then
    # bad syntax: input redir after pipe
    echo TEST 24
    printf "cat ../tests.sh | sort <tests.sh\nexit\n" | ./mish
    addgrade 'bad syntax: input redir after pipe'
    echo
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 25 ] ; then
    # bad syntax: output redir before pipe
    echo TEST 25
    printf "ls  >/tmp/t1 | sort \nexit\n" | ./mish
    addgrade 'bad syntax: output redir before pipe'
    echo
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 26 ] ; then
    # command that fails
    echo TEST 26
    printf "/my/ls /goobers \nexit\n" | ./mish
    addgrade 'command that fails'
    echo
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 27 ] ; then
    # command in pipe fails
    echo TEST 27
    printf "ls | /my/sort | cat \nexit\n" | ./mish
    addgrade 'command in pipe fails'
    echo
fi

if [ `wc -l $GRADEFILE | cut -d " " -f 1` -lt 28 ] ; then
    # parallel command fails
    echo TEST 28
    printf "ls | /my/sort &\nexit\n" | ./mish
    addgrade 'parallel command fails'
    echo
fi



