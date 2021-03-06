# Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""FAFT config setting overrides for Link."""


class Values(object):
    """FAFT config values for Link."""
    firmware_screen = 7
    chrome_ec = True
    dark_resume_capable = True
    ec_capability = ['adc_ectemp', 'battery', 'charging',
                     'keyboard', 'lid', 'x86', 'thermal',
                     'usb', 'peci', 'kblight', 'smart_usb_charge']
    spi_voltage = 'pp3300'
    smm_store = False
