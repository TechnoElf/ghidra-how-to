{
  description = "ghidra-how-to";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    typix = {
      url = "github:loqusion/typix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    typst-packages = {
      url = "github:typst/packages";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, flake-utils, typix, typst-packages }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };

        tx = typix.lib.${system};
        typstPackagesSrc = "${typst-packages}/packages";
        typstPackagesCache = pkgs.stdenv.mkDerivation {
          name = "typst-packages-cache";
          src = typstPackagesSrc;
          dontBuild = true;
          installPhase = ''
            mkdir -p "$out/typst/packages"
            cp -LR --reflink=auto --no-preserve=mode -t "$out/typst/packages" "$src"/*
          '';
        };
      in
      {
        packages.default = pkgs.symlinkJoin {
          name = "ghidra-how-to";
          paths = [
            self.packages.${system}.exercises
            self.packages.${system}.presentation
          ];
        };

        packages.exercises = pkgs.stdenv.mkDerivation {
          name = "ghidra-how-to-exercises";
          src = ./.;
          nativeBuildInputs = with pkgs; [ gnumake gcc ];
          buildPhase = ''
            make exercises
          '';
          installPhase = ''
            mkdir -p $out
            cp exercises/build/*.elf $out/
            cp exercises/build/*.bin $out/
          '';
        };

        packages.presentation = tx.buildTypstProject {
          src = ./.;
          XDG_CACHE_HOME = typstPackagesCache;
          typstSource = "presentation/main.typ";
          typstOpts = {
            root = "./.";
          };
          installPhaseCommand = ''
            mv $out out.pdf
            mkdir -p $out
            mv out.pdf $out/presentation.pdf
          '';
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gnumake
            gcc
            typst
          ];
        };
      }
    );
}
