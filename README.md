# MFA token generator: WIP

## Motivation:

To be able to generate MFA tokens from the command line.

This program's intended use is to store a list of access tokens in an encrypted file. Then when you need an mfa token specify the name of the service/company you need, enter a password and out pops an mfa token.

The idea is to use something that looks like this:
```
Qasd8238\asd23\A23eqaa==
```

To create an mfa token:

```
789821
```

From the command line for multiple services. As an alternative to something like Authy.

## Usage:

### Get token

Example to get an mfa token

```
$ generate_mfa -g aws
```

`-g` denotes get, followed by the name of the service/company/etc... name

This will prompt for the password used to encrypt the file containing your keys

```
Password: 
```

On success a `6` digit mfa token will be generated e.g: `786124`

### Print usage

```
$ generate_mfa -h
```

### Set a secret key

Example to set an access key by which to generate authentication tokens.

```
$ generate_mfa -s aws
```

`-s` denotes set, followed by the name of the service/company/etc... to store the key against

This will then prompt for your key to generate multi factor authentication tokens along with a password to encrypt the file:

```
Secret key to store: 
Password: 
```


## Installation

You will need the openssl headers and oath headers to build the code. This (2021-04-08) has only been tested on:
- Fedora Linux

```
# Ubuntu
sudo apt-get install libssl-dev
sudo apt-get install liboath-dev

# Fedora
sudo dnf install openssl-devel
sudo dnf install liboath-devel

# Mac untested
brew install openssl && brew link openssl --force
brew install oath-toolkit
```

Once the headers are installed:

```
$ mkdir -p build

$ make
```

## Security: 

- The access tokens are stored in a file which is encrypted using openssl's aes_256_cbc algorithm to ensure they are safely stored.
- The echo has been turned off for entering passwords and the key to store.
- Initalization vector has not been used.
- The whole file is encrypted / decrypted when generating an MFA token


## Compatability

This has been tested so far on:
- Linux Fedora
