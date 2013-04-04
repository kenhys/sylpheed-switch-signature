# Switch Signatures

Switch Signatures is a plugin for Sylpheed.

It implements simple interface to switch multiple 
pre-configured signatures easily associated for your accounts.

# How to use

Switch Signatures plugin adds functionality to swich
multiple signatures by clicking signature button on mail
compose windows.

# How to install

Clone sylpheed-swich-signatures repository.

```
% git clone https://github.com/kenhys/sylpheed-switch-signature.git
% git submodule init
% git submodule update
```

Then execute following commands

```
% ./autogen.sh
% ./configure --with-sylpheed-build-dir=/path/to/sylpheed_source_directory
% make
% make install
```

# TODO

* Implement missing functionality
  - Edit multiple signatures
  - Manage multiple signatures
