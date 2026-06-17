#!/usr/bin/bash

echo "#!/usr/bin/bash" > ~/pass.sh
echo "echo 'z@tanre.cn'" >> ~/pass.sh
chmod +x ~/pass.sh
set +e
# SUDO_ASKPASS=~/pass.sh sudo -A ip route delete default
# SUDO_ASKPASS=~/pass.sh sudo -A ip route delete default
# SUDO_ASKPASS=~/pass.sh sudo -A ip route delete default
SUDO_ASKPASS=~/pass.sh sudo -A rm bird.log
SUDO_ASKPASS=~/pass.sh sudo -A ./bird -l