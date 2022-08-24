#!/usr/bin/env bash
set -e -o pipefail

cd "$(dirname "${BASH_SOURCE[0]}")"

REPOSITORY="https://github.com/OneKeyHQ/onekey-firmware.git"
CONTAINER_NAME="lyxyx/onekey-build"
FAMILY=""
BRANCH=""
TAG=""

# stat under macOS has slightly different cli interface
USER=$(stat -c "%u" . 2>/dev/null || stat -f "%u" .)
GROUP=$(stat -c "%g" . 2>/dev/null || stat -f "%g" .)
DIR=$(pwd)

# pull docker image
docker pull lyxyx/onekey-build

function print_usage() {
  echo "Usage: ./build-docker.sh -f <family name> -t <tag>"
  echo "       -f family name <touch|mini|classic>"
  echo "       -t tag"
  echo "example:"
  echo "   ./build-docker.sh -f touch"
  echo "   ./build-docker.sh -f mini -t mini/v2.3.0"
  echo "   ./build-docker.sh -f classic -t classic/v2.3.0"
}


function build_touch() {
  mkdir -p build/touch

  PULL_SCRIPT_NAME=".nix.sh"
  BUILD_SCRIPT_NAME=".build.sh"

cat <<EOF > "build/$PULL_SCRIPT_NAME"
cd /tmp
git clone "$REPOSITORY" onekey-firmware
cd onekey-firmware
git checkout $TAG
chmod a+x /local/build/$BUILD_SCRIPT_NAME
/root/.nix-profile/bin/nix-shell --run /local/build/$BUILD_SCRIPT_NAME
EOF

cat <<EOF > "build/$BUILD_SCRIPT_NAME"
set -e -o pipefail
poetry install
git submodule update --init --recursive
cd core
ln -s /local/build/touch build
poetry run make build_embed
chown -R $USER:$GROUP build
EOF

  echo
  echo ">>> DOCKER RUN build touch"
  echo
  echo "tag $TAG"
  echo

  docker run -it --rm \
      -v "$DIR:/local" \
      -v "$DIR/build":/build:z \
      --init \
      --user root \
      "$CONTAINER_NAME" \
      bash /local/build/$PULL_SCRIPT_NAME
}

function build_mini() {
  mkdir -p build/mini

  PULL_SCRIPT_NAME=".pull.sh"
  BUILD_SCRIPT_NAME=".build.sh"

cat <<EOF > "build/$PULL_SCRIPT_NAME"
cd /tmp
git clone -b mini "$REPOSITORY" onekey-firmware
cd onekey-firmware
git checkout $TAG
chmod a+x /local/build/$BUILD_SCRIPT_NAME
/root/.nix-profile/bin/nix-shell --run /local/build/$BUILD_SCRIPT_NAME
EOF

cat <<EOF > "build/$BUILD_SCRIPT_NAME"
set -e -o pipefail
poetry install
export ONEKEY_MINI=1
cd legacy
poetry run make vendor
poetry run ./script/setup
poetry run ./script/cibuild
cp ./firmware/mini*Stable*.bin /local/build/mini
chown -R $USER:$GROUP /local/build
EOF

  echo
  echo ">>> DOCKER RUN build mini"
  echo
  echo "tag $TAG"
  echo

  docker run -it --rm \
      -v "$DIR:/local" \
      -v "$DIR/build":/build:z \
      --init \
      --user root \
      "$CONTAINER_NAME" \
      bash /local/build/$PULL_SCRIPT_NAME
}

function build_classic() {
  mkdir -p build/classic

  PULL_SCRIPT_NAME=".pull.sh"
  BUILD_SCRIPT_NAME=".build.sh"

cat <<EOF > "build/$PULL_SCRIPT_NAME"
set -e -o pipefail
cd /tmp
git clone -b bixin_dev "$REPOSITORY" onekey-firmware
cd onekey-firmware
git checkout $TAG
chmod a+x /local/build/$BUILD_SCRIPT_NAME
/root/.nix-profile/bin/nix-shell --run /local/build/$BUILD_SCRIPT_NAME
EOF

cat <<EOF > "build/$BUILD_SCRIPT_NAME"
set -e -o pipefail
poetry install
cd legacy
poetry run make vendor
poetry run ./script/setup
poetry run ./script/cibuild
cp ./firmware/classic*Stable*.bin  /local/build/classic
chown -R $USER:$GROUP /local/build
EOF

  echo
  echo ">>> DOCKER RUN build classic"
  echo
  echo "tag $TAG"
  echo

  docker run -it --rm \
      -v "$DIR:/local" \
      -v "$DIR/build":/build:z \
      --init \
      --user root \
      "$CONTAINER_NAME" \
      bash /local/build/$PULL_SCRIPT_NAME
}

# -f : Family name
# -t : Tag
while getopts 'f:t:' OPT; do
  case $OPT in
    f)
      FAMILY="$OPTARG";;
    t)
      TAG="$OPTARG";;
    ?)
      print_usage
      exit -1;;
  esac
done

if [ "$FAMILY" == "touch" ]; then
  build_touch
elif [ "$FAMILY" == "mini" ]; then
  build_mini
elif [ "$FAMILY" == "classic" ]; then
  build_classic
else
  echo "Invalid family name $IN_IMAGE."
  print_usage
  exit -1
fi

exit 0

