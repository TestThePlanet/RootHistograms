chmod +x ./go.sh
chmod +x ./makeAllPlots_nocrop.sh
chmod +x ./makeAllPlots.sh
chmod +x ./send.sh
alias commit='source "$(pwd)"/commit.sh'
alias go='source "$(pwd)"/go.sh'
alias save='source "$(pwd)"/send.sh'
alias send='source "$(pwd)"/send.sh'
alias commit='source `pwd`/commit.sh'
if [ $(grep -c "^[^#]alias go=[^#]go.sh" ~/.bashrc) -eq 1 ]; then
    echo "alias go='source /go.sh'" >> ~/.bashrc
fi
if [ $(grep -c "^[^#]alias save=[^#]send.sh" ~/.bashrc) -eq 1 ]; then
    echo "alias save='source /send.sh'" >> ~/.bashrc
fi
if [ $(grep -c "^[^#]alias send=[^#]send.sh" ~/.bashrc) -eq 1 ]; then
    echo "alias send='source /send.sh'" >> ~/.bashrc
fi
