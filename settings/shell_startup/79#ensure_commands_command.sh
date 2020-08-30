# create the "commands" command if it doesnt exist
if ! [[ -f "./commands/commands" ]]; then
    echo "#!/usr/bin/env bash
    echo \"project commands:\"
    ls -1 ./commands | sed 's/^/    /'
    " > "./commands/commands"
fi