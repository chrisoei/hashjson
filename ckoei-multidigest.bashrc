
for x in "/cygdrive/c/Users/software/Documents/My Dropbox/2009-06-25/Data" "$HOME/Dropbox/2009-06-25/Data" "$HOME/git/data"
do
	if [ -e "$x" ]
	then
		export CKOEI_DATA_HOME="$x"
		break
	fi
done

export CKOEI_MULTIDIGEST_DATA_HOME="$CKOEI_DATA_HOME/ckoei-multidigest"

if [ ! -e "$CKOEI_MULTIDIGEST_DATA_HOME" ]
then
	echo "WARNING: Unable to locate directory: $CKOEI_MULTIDIGEST_DATA_HOME"
fi

alias chk.anime='export CKOEI_MULTIDIGEST_DB="$CKOEI_MULTIDIGEST_DATA_HOME/anime.sqlite3"'
alias chk.comic='export CKOEI_MULTIDIGEST_DB="$CKOEI_MULTIDIGEST_DATA_HOME/comic.sqlite3"'
alias chk.itunes='export CKOEI_MULTIDIGEST_DB="$CKOEI_MULTIDIGEST_DATA_HOME/iTunes.sqlite3"'
alias chk.icsi='export CKOEI_MULTIDIGEST_DB="$CKOEI_MULTIDIGEST_DATA_HOME/icsi.sqlite3"'
alias chk.nestria='export CKOEI_MULTIDIGEST_DB="$CKOEI_MULTIDIGEST_DATA_HOME/nestria.sqlite3"'
alias chk.image='export CKOEI_MULTIDIGEST_DB="$CKOEI_MULTIDIGEST_DATA_HOME/image.sqlite3"'
alias chk.veracity='export CKOEI_MULTIDIGEST_DB="$CKOEI_MULTIDIGEST_DATA_HOME/veracity.sqlite3"'
alias chk.vb='export CKOEI_MULTIDIGEST_DB="$CKOEI_MULTIDIGEST_DATA_HOME/VirtualBox/`hostname`.sqlite3"'

alias chk.cd='cd "$CKOEI_MULTIDIGEST_DATA_HOME"'
alias chk.echo='echo "$CKOEI_MULTIDIGEST_DB"'
alias chk.a='ckoei-multidigest -a'
alias chk.f='ckoei-multidigest -f'
alias chk.n='ckoei-multidigest -n'
alias chk.q='ckoei-multidigest -q'
alias chk.sqlite='sqlite3 "$CKOEI_MULTIDIGEST_DB"'


