# Lets setup some definitions
let
    # niv should pin your current thing inside ./nix/sources
    # here we go and get that pinned version so we can pull packages out of it
    sources = import ./settings/nix/sources.nix;
    normalPackages = import sources.nixpkgs {};
    
    olderPackageRepoWithGcc = import (builtins.fetchGit {
         name = "packageRepoThatHasGcc-8.3.0";
         url = "https://github.com/nixos/nixpkgs-channels/";                       
         ref = "refs/heads/nixpkgs-unstable";                     
         rev = "a9eb3eed170fa916e0a8364e5227ee661af76fde";                                           
     }) {};                                                                           

# using those definitions
in
    # create a shell
    normalPackages.mkShell {
        
        # inside that shell, make sure to use these packages
        buildInputs = [
            olderPackageRepoWithGcc.gcc-unwrapped
            normalPackages.cmake
            normalPackages.gtk2-x11
            # python and venv
            normalPackages.python37
            normalPackages.python37Packages.setuptools
            normalPackages.python37Packages.pip
            normalPackages.python37Packages.virtualenv
            # basic commandline tools
            normalPackages.ripgrep
            normalPackages.which
            normalPackages.git
            normalPackages.colorls
            normalPackages.tree
            normalPackages.less
            normalPackages.niv
            normalPackages.cacert # needed for niv
            normalPackages.nix    # needed for niv
            # 
            # how to add packages?
            # 
            # to find package verisons use:
            #     nix-env -qP --available PACKAGE_NAME_HERE | cat
            # ex:
            #     nix-env -qP --available opencv
            # to add those specific versions find the nixpkgs.STUFF 
            # and add it here^ as normalPackages.STUFF
            # ex find:
            #     nixpkgs.python38Packages.opencv3  opencv-3.4.8
            # ex add:
            #     normalPackages.python38Packages.opencv3
            # 
            # NOTE: some things (like setuptools) just don't show up in the 
            # search results for some reason, and you just have to guess and check 🙃 
        ];
        
        shellHook = ''
        # 
        # find and run all the startup scripts in alphabetical order
        # 
        for file in ./settings/shell_startup/*
        do
            # make sure its a file
            if [[ -f $file ]]; then
                source $file
            fi
        done
        '';
    }

