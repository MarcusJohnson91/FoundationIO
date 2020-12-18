CommitID=$(git rev-parse $(git branch --show-current))
CommitID_Caps=$(echo $CommitID | tr '[0-9a-f]' '[0-9A-F]')

printf "#define Version_CommitID %s\n" "$CommitID_Caps" >$(pwd)/../Library/include/Private/CommitID.h
