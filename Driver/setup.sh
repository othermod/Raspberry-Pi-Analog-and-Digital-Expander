#!/bin/bash

# https://www.othermod.com

function check_root() {
    if [ $(id -u) -ne 0 ]; then
        echo "Installer must be run as root."
        echo "Try 'sudo bash $0'"
        exit 1
    fi
}

function check_files_exist() {
    # Paths to the files
    local file_paths=("gamepad.c" "scan.c" "gamepad.service")

    # Check if the files exist
    for file_path in "${file_paths[@]}"; do
        if [ ! -f "$file_path" ]; then
            echo "File $file_path not found!"
            exit 1
        fi
    done
}

function enable_i2c() {
    local INTERACTIVE=False
    local BLACKLIST=/etc/modprobe.d/raspi-blacklist.conf
    local CONFIG=/boot/config.txt

    echo "Enabling I2C"

    SETTING=on
    STATUS=enabled

    #set_config_var dtparam=i2c_arm $SETTING $CONFIG &&
    if ! [ -e $BLACKLIST ]; then
        touch $BLACKLIST
    fi
    sed $BLACKLIST -i -e "s/^\(blacklist[[:space:]]*i2c[-_]bcm2708\)/#\1/"
    sed /etc/modules -i -e "s/^#[[:space:]]*\(i2c[-_]dev\)/\1/"
    if ! grep -q "^i2c[-_]dev" /etc/modules; then
        printf "i2c-dev\n" >> /etc/modules
    fi
    dtparam i2c_arm=$SETTING
    modprobe i2c-dev

    grep 'dtparam=i2c_arm' /boot/config.txt >/dev/null
    if [ $? -eq 0 ]; then
        sudo sed -i '/dtparam=i2c_arm/c\dtparam=i2c_arm' /boot/config.txt
    else
        echo "dtparam=i2c_arm=on" >> /boot/config.txt
    fi
    echo "Adding i2c_vc to config.txt"
    grep 'dtparam=i2c_vc' /boot/config.txt >/dev/null
    if [ $? -eq 0 ]; then
      sudo sed -i '/dtparam=i2c_vc/c\dtparam=i2c_vc=on' /boot/config.txt
    else
      echo "dtparam=i2c_vc=on" >> /boot/config.txt
    fi
}

function compile_and_run_i2c_scanner() {
    echo "Compiling the I2C scanner"
    rm scan 2>/dev/null
    gcc -o scan scan.c
    if [ $? -eq 0 ]; then
        echo "Scanning every I2C bus to find the two modules"
    else
        echo "Failed to compile the I2C scanner"
				echo "Make sure you copied the correct scan.c file"
        exit 1
    fi
    ./scan
}

function get_and_set_i2c_bus() {
    # Scan /dev for available I2C buses
    available_buses=($(ls /dev/i2c-* 2>/dev/null | sed 's/\/dev\/i2c-//'))

    # Check if any I2C buses were found
    if [ ${#available_buses[@]} -eq 0 ]; then
        echo "No I2C buses found"
        echo "Try rebooting and run the script again"
        exit 1
    fi

    echo "List of available I2C buses: ${available_buses[@]}"
    echo "---------------------------------------------"
    echo "Using the above results as a guide, enter the number of the I2C bus"
    echo "The default is 1, but it depends on your setup"

    while true; do
    # Ask the user to input a value from the available buses
    read -p "Enter a number from the available buses: " input_value

    # Check if the input_value is in the list of available buses
    if [[ " ${available_buses[@]} " =~ " ${input_value} " ]]; then
        break
    else
        echo "Invalid input. The input should be a number from the available buses. Please try again."
    fi
    done

    for file in gamepad.c datareader.c; do
    # Check for the existence of the line in the file
    if grep -q "#define I2C_BUS \"/dev/i2c-" "$file"; then
        # Use sed command to replace the line in the file
        sed -i "s|#define I2C_BUS \"/dev/i2c-.*\"|#define I2C_BUS \"/dev/i2c-$input_value\"|g" "$file"
        echo "The file $file has been updated successfully"
    else
        echo "The line '#define I2C_BUS \"/dev/i2c-' does not exist in $file. Make sure you copied the correct file."
        exit 1
    fi
    done
}


function install_service() {
    # Ask the user for the number of joysticks
    while true; do
        read -p "Enter the number of joysticks to enable (0-3): " num_joysticks
        if [[ "$num_joysticks" =~ ^[0-3]$ ]]; then
            break
        else
            echo "Invalid input. Please enter a number between 0 and 3."
        fi
    done

    # Modify the gamepad.service file
    # Replace the entire ExecStart line with the new one
    sed -i "/ExecStart/c\ExecStart=sudo gamepad $num_joysticks" gamepad.service
    remove_existing_gamepad_service
    echo "Copying new driver and service files"
    cp -f gamepad /usr/bin/gamepad
    cp -f gamepad.service /etc/systemd/system/gamepad.service
    echo "Enabling gamepad service"
    #having occasional issues with the enabling, and i have to run it again. try sudo? keep an eye on this
    sudo systemctl enable gamepad
    sudo systemctl start gamepad
}


function compile_drivers() {
    for file in gamepad datareader; do
        echo "Compiling the $file driver"
        rm $file 2>/dev/null

        # Compile the C file
        gcc -O3 -o $file $file.c
        if [ $? -eq 0 ]; then
            # copy to the /usr/bin folder
            cp -f $file /usr/bin/$file
            echo "The $file was compiled successfully"
        else
            echo "Failed to compile the $file driver. Make sure you copied the correct $file.c file."
            exit 1
        fi
    done
}

function remove_existing_gamepad_service() {
    echo "Disabling and removing existing gamepad service, if it exists"
    sudo killall gamepad 2>/dev/null
    sudo systemctl stop gamepad 2>/dev/null
    sudo systemctl disable gamepad 2>/dev/null
}

function prompt_for_autostart() {
    echo "Do you want the gamepad driver to load at startup?"
    PS3='Enter the number of your choice: '
    options=("Yes" "No")
    select yn in "${options[@]}"
    do
        case $yn in
            "Yes")
                install_service
                echo "The gamepad should now be running"
                echo "If it is not functioning, then try a reboot"
                break;;
            "No")
                remove_existing_gamepad_service
                break;;
            *)
                echo "Invalid option. Please enter the number corresponding to 'Yes' or 'No'.";;
        esac
    done
}

# Call functions in the right order
check_root
check_files_exist
enable_i2c
compile_and_run_i2c_scanner
get_and_set_i2c_bus
compile_drivers
prompt_for_autostart
