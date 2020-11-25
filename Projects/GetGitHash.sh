CommitID=$(git rev-parse $(git branch --show-current))
CommitID_Caps=$(echo $CommitID | tr '[a-z]' '[A-Z]')

printf "#define Version_CommitID %s\n" "$CommitID_Caps" >$(pwd)/../Library/include/Private/CommitID.h
