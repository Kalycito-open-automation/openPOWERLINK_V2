################################################################################
#
# CMake global settings file for target Xilinx ARM Core
#
# Copyright (c) 2014, Kalycito Infotech Pvt. Ltd.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the copyright holders nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
################################################################################

###############################################################################
# Handle ARM settings
IF(CFG_DEMO_BUS_SYSTEM MATCHES axi)
    SET(XIL_CFLAGS "${XIL_CFLAGS} -mlittle-endian")
    SET(XIL_PLAT_ENDIAN -mlittle-endian)
ELSE()

ENDIF()

IF(CFG_ARM_FRAME_AAPCS)
    SET(XIL_CFLAGS "${XIL_CFLAGS} -mapcs-frame")
ELSE()
    SET(XIL_CFLAGS "${XIL_CFLAGS} -mno-apcs-frame")
ENDIF()

if(CFG_ARM_SHORT_ENUMS)
    SET(XIL_CFLAGS "${XIL_CFLAGS} -fno-short-enums")
ELSE()
    SET(XIL_CFLAGS "${XIL_CFLAGS} -fshort-enums")
ENDIF()