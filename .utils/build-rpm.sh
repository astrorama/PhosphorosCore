#!/bin/bash
set -ex

# Environment
export VERBOSE=1
export CTEST_OUTPUT_ON_FAILURE=1
export BRANCH=$1
export LC_ALL=C

# Platform-specific configuration
source /etc/os-release

CMAKEFLAGS="-DINSTALL_DOC=ON"

if [ "$ID" == "fedora" ]; then
  if [ "$VERSION_ID" -ge 30 ]; then
    PYTHON="python3"
    CMAKEFLAGS="$CMAKEFLAGS -DPYTHON_EXPLICIT_VERSION=3"
  else
    PYTHON="python"
  fi
elif [ "$ID" == "centos" ]; then
  yum install -y epel-release
  PYTHON="python"
fi

# Git may not be available
yum install -y git

# Astrorama repository
cat > /etc/yum.repos.d/astrorama.repo << EOF
[artifactory--astrorama-fedora]
name=artifactory--astrorama-fedora
baseurl=https://astrorama.jfrog.io/artifactory/rpm/stable/${ID}/\$releasever/\$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1
EOF

if [ "${BRANCH}" == "develop" ]; then
  cat >> /etc/yum.repos.d/astrorama.repo <<EOF
[artifactory--astrorama-fedora-develop]
name=artifactory--astrorama-fedora-develop
baseurl=https://astrorama.jfrog.io/artifactory/rpm/develop/${ID}/\$releasever/\$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1
EOF
  REL_NUMBER=$(date +%Y%m%d%H%M)
  CMAKEFLAGS="$CMAKEFLAGS -DCPACK_PACKAGE_RELEASE=r${REL_NUMBER}"
fi

# From the CMakeLists.txt, retrieve the list of dependencies
cmake_deps=$(grep -oP 'elements_project\(\S+\s+\S+ USE \K(\S+ \S+)*(?=\))' /src/CMakeLists.txt)
rpm_dev_deps=$(echo ${cmake_deps} | awk '{for(i=1;i<NF;i+=2){print $i "-devel-" $(i+1)}}')
rpm_doc_deps=$(echo ${cmake_deps} | awk '{for(i=1;i<NF;i+=2){print $i "-doc-" $(i+1)}}')
yum install -y ${rpm_dev_deps} ${rpm_doc_deps}

# Dependencies
yum install -y cmake make gcc-c++ rpm-build
yum install -y boost-devel $PYTHON-pytest log4cpp-devel doxygen CCfits-devel
yum install -y graphviz $PYTHON-sphinx $PYTHON-sphinxcontrib-apidoc

# Build
mkdir -p /build
cd /build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DINSTALL_TESTS=OFF -DRPM_NO_CHECK=OFF $CMAKEFLAGS /src
make $MAKEFLAGS rpm

