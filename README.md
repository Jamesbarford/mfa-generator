# MFA token generator: WIP

A simple command line program for storing a list of access tokens which can have multi factor authentication tokens generated against. The access tokens are sored in a file which is encrypted using openssl's aes_256_cbc algorithm to ensure they are safely stored. The echo has been turned off for entering passwords and the key to store.

The idea is to turn something that looks like this:
```
Qasd8238\asd23\A23eqaa==
```

Into an mfa token:

```
789821
```

From the command line for multiple services.

## Usage:

### Print usage

```
$ generate_mfa -h
```

### Set token

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

# Compatability

This has been tested so far on:
- Linux
