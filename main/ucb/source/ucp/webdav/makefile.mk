#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

# UCP Version - Increase, if UCP libraray becomes incompatible.
UCP_VERSION=1

# Name for the UCP. Will become part of the library name (See below).
UCP_NAME=dav

# Relative path to project root.
PRJ = ..$/..$/..

# Project Name.
PRJNAME=ucb

TARGET=ucp$(UCP_NAME)

ENABLE_EXCEPTIONS=TRUE
USE_DEFFILE=TRUE
NO_BSYMBOLIC=TRUE

# --- Settings ---------------------------------------------------------

.INCLUDE: settings.mk
.IF "$(L10N_framework)"==""

APRINCDIR=apr
APRUTILINCDIR=apr-util
SERFINCDIR=serf

#.IF "$(SYSTEM_APR)" != "YES"
#.INCLUDE: $(SOLARINCDIR)$/$(APRINCDIR)$/version.mk
#.ENDIF
#.IF "$(SYSTEM_APRUTIL)" != "YES"
#.INCLUDE: $(SOLARINCDIR)$/$(APRUTILINCDIR)$/version.mk
#.ENDIF
#.IF "$(SYSTEM_SERF)" != "YES"
#.INCLUDE: $(SOLARINCDIR)$/$(SERFINCDIR)$/version.mk
#.ENDIF

#CFLAGS+= -DAPR_VERSION=0x$(APR_VERSION) -DAPRUTIL_VERSION=0x$(APRUTIL_VERSION) -DSERF_VERSION=0x$(SERF_VERSION)

#
# Extend the list of include paths depending on whether we use locally built
# or system versions of libraries apr, apr-util, serf, libxml
#
# We have to use CFLAGS for this because PRJINC is too inflexible (it adds /inc to everyting.)
#

.IF "$(SYSTEM_APR)" == "YES"
CFLAGS+= $(APR_CFLAGS)
.ELSE
CFLAGS+= -I$(SOLARINCDIR)$/$(APRINCDIR)
.ENDIF

.IF "$(SYSTEM_APRUTIL)" == "YES"
CFLAGS+= $(APRUTIL_CFLAGS)
.ELSE
CFLAGS+= -I$(SOLARINCDIR)$/$(APRUTILINCDIR)
.ENDIF

.IF "$(SYSTEM_SERF)" == "YES"
CFLAGS+= $(SERF_CFLAGS)
.ELSE
CFLAGS+= -I$(SOLARINCDIR)$/$(SERFINCDIR)
.ENDIF

.IF "$(SYSTEM_LIBXML)" == "YES"
CFLAGS+= $(LIBXML_CFLAGS)
.ELSE
LIBXMLINCDIR=external$/libxml
CFLAGS+= -I$(SOLARINCDIR)$/$(LIBXMLINCDIR)
.ENDIF

.IF "$(SYSTEM_OPENSSL)" == "YES"
CFLAGS+= -I$(OPENSSL_CFLAGS)
.ENDIF

# --- General -----------------------------------------------------

SLOFILES=\
    $(SLO)$/webdavservices.obj \
    $(SLO)$/webdavprovider.obj \
    $(SLO)$/webdavcontent.obj \
    $(SLO)$/webdavcontentcaps.obj \
    $(SLO)$/webdavresultset.obj \
    $(SLO)$/webdavdatasupplier.obj \
    $(SLO)$/ContentProperties.obj \
    $(SLO)$/DAVProperties.obj \
    $(SLO)$/DAVSessionFactory.obj \
    $(SLO)$/DAVResourceAccess.obj \
    $(SLO)$/AprEnv.obj \
    $(SLO)$/webdavresponseparser.obj \
    $(SLO)$/SerfUri.obj \
    $(SLO)$/SerfRequestProcessor.obj \
    $(SLO)$/SerfRequestProcessorImpl.obj \
    $(SLO)$/SerfRequestProcessorImplFac.obj \
    $(SLO)$/SerfPropFindReqProcImpl.obj \
    $(SLO)$/SerfPropPatchReqProcImpl.obj \
    $(SLO)$/SerfGetReqProcImpl.obj \
    $(SLO)$/SerfHeadReqProcImpl.obj \
    $(SLO)$/SerfPutReqProcImpl.obj \
    $(SLO)$/SerfPostReqProcImpl.obj \
    $(SLO)$/SerfDeleteReqProcImpl.obj \
    $(SLO)$/SerfMkColReqProcImpl.obj \
    $(SLO)$/SerfCopyReqProcImpl.obj \
    $(SLO)$/SerfMoveReqProcImpl.obj \
    $(SLO)$/SerfSession.obj \
    $(SLO)$/SerfCallbacks.obj \
    $(SLO)$/SerfInputStream.obj \
    $(SLO)$/DateTimeHelper.obj \
    $(SLO)$/UCBDeadPropertyValue.obj

LIB1TARGET=$(SLB)$/_$(TARGET).lib
LIB1OBJFILES=$(SLOFILES)

# --- Shared-Library ---------------------------------------------------

SHL1TARGET=$(TARGET)$(UCP_VERSION)
SHL1IMPLIB=i$(TARGET)

SHL1VERSIONMAP=$(SOLARENV)/src/component.map

SHL1STDLIBS=\
        $(CPPUHELPERLIB) \
        $(CPPULIB)       \
        $(SALLIB)        \
        $(SALHELPERLIB)  \
        $(UCBHELPERLIB)  \
        $(COMPHELPERLIB) \
		$(SERFLIBS)      \
        $(LIBXML2LIB)

.IF "$(GUI)"=="WNT"
SHL1STDLIBS+= $(WSOCK32LIB)
.IF "$(WINDOWS_VISTA_PSDK)" != ""
SHL1STDLIBS+= $(WS2_32LIB)
.ENDIF
SHL1STDLIBS+= $(OPENSSLLIB)
.ELSE # WNT
.IF "$(OS)"=="SOLARIS"
SHL1STDLIBS+= -lnsl -lsocket -ldl
.ENDIF # SOLARIS
.IF "$(OS)"=="OS2"
SHL1STDLIBS+= pthread.lib libz.lib
.ENDIF # OS2
.IF "$(SYSTEM_OPENSSL)"=="YES"
SHL1STDLIBS+= $(OPENSSLLIB)
.ELSE
SHL1STDLIBS+= $(OPENSSLLIBST)
.ENDIF
.ENDIF # WNT

SHL1DEF=$(MISC)$/$(SHL1TARGET).def
SHL1LIBS=$(LIB1TARGET)

# --- Def-File ---------------------------------------------------------

DEF1NAME=$(SHL1TARGET)

.ENDIF # L10N_framework
# --- Targets ----------------------------------------------------------

.INCLUDE: target.mk

ALLTAR : $(MISC)/ucpdav1.component

$(MISC)/ucpdav1.component .ERRREMOVE : $(SOLARENV)/bin/createcomponent.xslt \
        ucpdav1.component
    $(XSLTPROC) --nonet --stringparam uri \
        '$(COMPONENTPREFIX_BASIS_NATIVE)$(SHL1TARGETN:f)' -o $@ \
        $(SOLARENV)/bin/createcomponent.xslt ucpdav1.component
