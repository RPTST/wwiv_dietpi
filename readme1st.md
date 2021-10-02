    
I have compliled WWIV on a Raspberry Pi 4 1GB using dietpi version ARMv8 (Bullseye) image.  Here is where I can see the upgrades and changes done to the source and verify it could be done.

    https://build.wwivbbs.org/jenkins/job/wwiv/label=linux-arm64-raspbian/

The following documentation is what I used to assist in compalation.

    https://github.com/wwivbbs/wwiv#building-on-linux
    http://docs.wwivbbs.org/en/latest/install/linux/

Useless documentation as the pages go no where as the site looks broken.

    http://docs.wwivbbs.org/en/latest/linux_installation/





#    Setup system

Downloaded ISO - 64-bit

    https://downloads.raspberrypi.org/raspios_lite_arm64/images/






#    Refresh your OS

    sudo apt-get update
    sudo apt-get upgrade -y
    sudo reboot






#    Install python

    sudo apt-get install python3 python3-pip
    sudo apt-get install python python-pip

Or use python2 depending on what your distrobution expects.

    sudo apt-get install python2 python2-pip

If pip will not install for python/2 then use the below command to download to your system

    curl https://bootstrap.pypa.io/pip/2.7/get-pip.py --output get-pip.py
    sudo python2 get-pip.py






#    Install essentials

    sudo apt install -y build-essential make cmake gcc g++ git curl wget htop ninja-build neofetch default-jdk default-jre mosh screen bmon wavemon iftop ipcalc hexcurse exa hping3 arping lshw fping chkrootkit ioping trash-cli ranger mc whowatch lsof nethogs fdupes stress ccze tilde nmap original-awk gawk curl git wget zip unzip unrar-free tshark saidar xlsx2csv docx2txt pwgen libcrack2
    sudo reboot






#    Create and move to temp directory

    mkdir ~/projects
    cd ~/projects






#    Install bpytop (a much better and more colorful htop command)

    pip3 install bpytop





#    Install LSD a more colorful LS

    wget https://github.com/Peltoche/lsd/releases/download/0.20.1/lsd-musl_0.20.1_arm64.deb
    nano ~/.bashrc

Then edit alias

    ls='ls' to alias ls='lsd'






#    Install SYNTH-SCHELL a very fancy command prompt

    git clone --recursive https://github.com/andresgongora/synth-shell.git
    chmod +x synth-shell/setup.sh
    cd synth-shell
    ./setup.sh

*At the prompt please enter /I /U /N /Y /N /N /N 





#    Setting up / Installing ZRAM (only needed if not using dietpi)

Remove the old dphys version

    sudo /etc/init.d/dphys-swapfile stop
    sudo apt-get remove --purge dphys-swapfile

Install zram

    sudo wget -O /usr/bin/zram.sh https://raw.githubusercontent.com/novaspirit/rpi_zram/master/zram.sh

Set autoload

    sudo nano /etc/rc.local

Add the next line before exit 0

    /usr/bin/zram.sh &

Save with <Ctrl+X>, <Y> and <Enter>

  /usr/bin/zram.sh &
  sudo chmod +x /usr/bin/zram.sh
  sudo nano /usr/bin/zram.sh

Alter the limit with * 2
  
  mem=$(( ($totalmem / $cores)* 1024 * 2))

Save with <Ctrl+X>, <Y> and <Enter>

    sudo reboot
 
Setting up / Installing ZRAM (if using dietpi)
  
Now we will be editing the configuration file.
  
    sudo nano /boot/diepi.txt
  
In /boot/diepi.txt change the "1" to your desired swap file size. I used 2GB instead of auto.
  
    AUTO_SETUP_SWAPFILE_SIZE=2048
  
Then reboot
 
    sudo reboot





#    Install RPI monitor
  
     sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-key 2C0D3C0F
     sudo wget http://goo.gl/vewCLL -O /etc/apt/sources.list.d/rpimonitor.list
     sudo apt update
     sudo apt install -y rpimonitor
  
Use the below to stop / start / restart
  
    sudo service rpimonitor stop
    sudo service rpimonitor start
    sudo service rpimonitor restart
    sudo service rpimonitor status
  
Open a browser and go to http://<IP_ADDRESS>:8888





#    Install bat a more IDE color style cat command
     Download 64-bit
  
     wget https://github.com/sharkdp/bat/releases/download/v0.18.3/bat_0.18.3_arm64.deb
  
     Download 32-bit
  
     wget https://github.com/sharkdp/bat/releases/download/v0.18.3/bat_0.18.3_armhf.deb

  
#    Install bat
  
     sudo dpkg -i bat_*.deb
     rm bat*.deb
     bat /etc/profile





#    Install ps-mem a more granular way to see what apps are using your memory more usefully.

     sudo wget -O /usr/local/bin/ps_mem https://raw.githubusercontent.com/pixelb/ps_mem/master/ps_mem.py
     sudo ps_mem






#    + Setting up box64
  
git clone https://github.com/ptitSeb/box64
  
cd box64; mkdir build; cd build
  
#    
#    Install Box64
#    
  
cmake .. -DRPI4ARM64=1 -DCMAKE_BUILD_TYPE=RelWithDebInfo
  
make -j$(nproc)
  
sudo make install
  
sudo systemctl restart systemd-binfmt
  

  
  
  
  
#    Install WWIV on linux
  
    mkdir ~/git/
  
    cd ~/git/
  
    sudo mkdir -p /opt/wwiv && sudo chown "$(id -u):$(id -g)" /opt/wwiv

    
#    Build from source
 
  git clone --recurse-submodules -j8 https://github.com/wwivbbs/wwiv.git
  
  cd wwiv/
  
  wget https://github.com/wwivbbs/wwiv/releases/download/wwiv_5.7.1/wwiv-linux-arm64-raspbian-5.7.1.3532.tar.gz
  
  mkdir _build
  
  cd _build 
  
  ../cmake-config.sh 
  
  cmake --build .
  
  cd ~/git/wwiv/
  
  mv README.md readme1st.txt

    
#    Moving the build from building directory to working directory

  mv ~/git/wwiv/* /opt/wwiv/
    
Or, copy all of the files newly built, or symlinks to them from your WWIV base install i.e. in /opt/wwiv/ (assuming the source is in $HOME/git/wwiv now

  cd /opt/wwiv
  
  ln -s $HOME/git/wwiv/builds/tools/linux/use-built-bin.sh
  
  export BUILT_BIN=$HOME/out/wwiv/_build/
  
  ./use-built-bin.sh ${BUILT_BIN} ~/git/wwiv/_build/bbs/bbs ~/git/wwiv/_build/wwivd/wwivd ~/git/wwiv/_build/wwivconfig/wwivconfig ~/git/wwiv/_build/wwivfsed/wwivfsed ~/git/wwiv/_build/wwivutil/wwivutil ~/git/wwiv/_build/network/network ~/git/wwiv/_build/network1/network1 ~/git/wwiv/_build/network2/network2 ~/git/wwiv/_build/network3/network3 ~/git/wwiv/_build/networkb/networkb ~/git/wwiv/_build/networkc/networkc ~/git/wwiv/_build/networkf/networkf ~/git/wwiv/_build/networkt/networkt

    
From the wwiv directory (i.e. /opt/wwiv), run sudo ./install.sh and follow the prompts.
There are many options available for install.sh, please run sudo ./install.sh  --help for the full list of options. By default the script expects the username and groupname to both be wwiv and not yet exist.
You must run this as root. The script performs the following tasks for you: creating the WWIV user and group setting up sudo access for a standard user installing the systemd service file.

  sudo ./install.sh --help
  
  sudo ./install.sh -n
  
  sudo ./install.sh
   
Now, you may log into the new wwiv user (e.g, sudo -u wwiv -s) and run ./wwivconfig to configure the BBS. (If that first sudo command doesn't work on your system, try sudo su - wwiv -s /bin/bash)
  
    sudo -u wwiv -s
 
If you have any issues, check the install_date_time.log file that was created during the install.  If you still can't tell what happened, come and find us in IRC or post a message in one of the wwiv support subs.

  
#    After the install
    
If you've gotten this far, Your BBS should be up and running. Everything below this point is details about more in-depth configuration (DOORs, WWIVnet, etc) and some of the current warts that linux has that you need to be aware of. If you come across anything that is not detailed here, please let us know.

  http://docs.wwivbbs.org/en/latest/chains/tradewars2002/
   

For more information on WWIV confifuration options / paramaters.

  http://docs.wwivbbs.org/en/latest/cfg/wwivconfig/
   


#    + Errors

Here are some of the common errors I encountered why compiling:

  ../core/parser/ast.cpp:172:10: warning: enumeration value ‘number’ not handled in switch [-Wswitch]
  ../core/parser/ast.cpp:172:10: warning: enumeration value ‘error’ not handled in switch [-Wswitch]
  ../core/parser/ast.cpp: In function ‘std::unique_ptr<wwiv::core::parser::BinaryOperatorNode> wwiv::core::parser::createBinaryOperator(const wwiv::core::parser::Token&)’:
  ../core/parser/ast.cpp:183:10: warning: enumeration value ‘lparen’ not handled in switch [-Wswitch]
      183 |   switch (token.type) {
          |          ^
  ../core/parser/ast.cpp:183:10: warning: enumeration value ‘rparen’ not handled in switch [-Wswitch]
  ../core/parser/ast.cpp:183:10: warning: enumeration value ‘negate’ not handled in switch [-Wswitch]
  ../core/parser/ast.cpp:183:10: warning: enumeration value ‘semicolon’ not handled in switch [-Wswitch]
