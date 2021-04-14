#!/usr/bin/env bash

# feed in qr image and get a key, only if there is no secret
function get_secret_from_qr() {
	local QR_IMG=$1
	zbarimg $QR_IMG >> out.txt
}

# if we have a secert this will generate an MFA
function genmfa_shell() {
	local SECRET=$1
	local KEY=$(oathtool --totp --base32 $SECRET)
	echo $KEY
}

