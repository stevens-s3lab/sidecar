#!/bin/sh

# Copyright (C) Internet Systems Consortium, Inc. ("ISC")
#
# SPDX-License-Identifier: MPL-2.0
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0.  If a copy of the MPL was not distributed with this
# file, you can obtain one at https://mozilla.org/MPL/2.0/.
#
# See the COPYRIGHT file distributed with this work for additional
# information regarding copyright ownership.

rm -f ns*/*.key ns*/*.private
rm -f ns2/tld2s.db */bl.tld2.db */bl.tld2s.db
rm -f ns3/bl*.db ns3/fast-expire.db ns*/empty.db
rm -f ns3/manual-update-rpz.db
rm -f ns3/mixed-case-rpz.db
rm -f ns5/example.db ns5/bl.db ns5/fast-expire.db ns5/expire.conf
rm -f ns8/manual-update-rpz.db
rm -f */policy2.db
rm -f */*.jnl
rm -f dnsrps.cache dnsrps.conf
rm -f proto.* dsset-* trusted.conf dig.out* nsupdate.tmp ns*/*tmp
rm -f ns5/requests ns5/*.perf
rm -f */named.memstats */*.run */*.run.prev */named.stats */session.key
rm -f */*.log */*core */*.pid
rm -f ns*/named.lock
rm -f ns*/named.conf
rm -f ns*/*switch
rm -f dnsrps.zones
rm -f ns*/managed-keys.bind*
rm -f tmp