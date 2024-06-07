# Notas de Deploy SSOO

Algunas de las cosas que se listan también sirven para poder trabajar sobre el repositorio de manera cómoda, no todo es exclusivo para el deploy para la presentación del TP.

## TODO
- Revisar permisos necesarios de administrador en Windows: en las máquinas de la facultad no tenemos :(
- Evaluar cómo hacer para pasar el repositorio y los scripts de deploy lo más fácil posible a las máquinas de la facultad
- Hacer un script/comando que habilite/deshabilite características de Windows
- Hacer un script que configure automáticamente la VM en VirtualBox, o llevar los archivos de configuración ya hechos
- Hacer un script que instale automáticamente las extensiones de VSCode
- Hacer un script que tome las IPs del Host y del Guest y que haga las conexiones SSH automáticamente y edite los .config acordemente
- Editar el archivo /home/utnso/base.sh acordemente
- Conexión a GitHub
- Sistema operativo Host utilizado en las computadoras de la facultad: Windows o Linux
- Acceso a GitHub desde las computadoras de la facultad
	- https://stackoverflow.com/questions/69336114/the-application-requires-one-of-the-following-versions-of-the-net-framework-ho

-----------------------------

# Al entrar

## 1. Encender e iniciar sesión en la computadora

Laboratorio de Sistemas UTN FRBA: 3er piso, ~Aula 317, Sede Medrano
- **Usuario**: alumno
- **Contraseña**: alumno

-----------------------------

## 2. Descargar Git Bash Portable

-----------------------------

## 3. Iniciar sesión en Git Bash

-----------------------------

## 4. Clonar este repositorio

-----------------------------

## 5. Descargar VirtualBox

- https://www.virtualbox.org/wiki/Downloads

-----------------------------

## 6. Descargar la VM de Ubuntu Server

Página oficial
- https://docs.utnso.com.ar/recursos/vms

Links de descarga
- https://drive.google.com/drive/folders/1Pn1SveTGkEVfcc7dYAr1Wc10ftEe8E0J
- https://drive.google.com/file/d/16lUW1fRvoitkUiUN58mwJrmyAVezcOdx/view
- https://drive.google.com/file/d/16lUW1fRvoitkUiUN58mwJrmyAVezcOdx/view?usp=drive_link

Gestor de descargas (Windows)
- https://www.freedownloadmanager.org/es/download.htm

Gestor de descargas (Ubuntu)
- https://chemicloud.com/blog/download-google-drive-files-using-wget/
- https://medium.com/@acpanjan/download-google-drive-files-using-wget-3c2c025a8b99
- https://stackoverflow.com/questions/25010369/wget-curl-large-file-from-google-drive
- https://superuser.com/questions/1518582/how-to-use-wget-to-download-a-file-stored-in-google-drive-without-making-publicl

-----------------------------

## 7. Características de Windows

- Desactivar `Plataforma de Hipervisor de Windows`

-----------------------------

## 8. Configuración de la VM (Ubuntu Server) en VirtualBox

- General
	- Básico
		- `Nombre`: Ubuntu Server 6.1
		- `Tipo`: Linux
		- `Versión`: Ubuntu (64-bit)
	- Avanzado
		- `Carpeta de instantáneas`: C:\Users\Usuario\VirtualBox VMs\Ubuntu Server 6.1\Snapshots
		- `Portapapeles compartido`: Inhabilitado
		- `Arrastrar y soltar`: Inhabilitado
	- Descripción
	- Cifrado de disco
		- [ ] Habilitar cifrado de disco

- Sistema
	- Placa base
		- `Memoria base`: 2048 MB
		- `Orden de arranque`: Óptica - Disco Duro
		- `Chipset`: PIIX3
		- `TPM`: Ninguno
		- `Dispositivo apuntador`: Tableta USB
		- `Características extendidas`:
			- [X] Habilitar I/O APIC
			- [X] Habilitar reloj hardware en tiempo UTC
			- [ ] Habilitar EFI (sólo SO especiales)
			- [ ] Habilitar Secure Boot
	- Procesador
		- `Procesadores`: 1
		- `Límite de ejecución`: 100%
		- `Características extendidas`:
			- [ ] Habilitar PAE/NX
			- [ ] Habilitar VT-x/AMD-V anidado
	- Aceleración
		- `Interfaz de paravirtualización`: Predeterminado
		- Hardware de virtualización:
			- [X] Habilitar paginación anidada

- Pantalla
	- Pantalla
		- `Memoria de video`: 16 MB
		- `Número de monitores`: 1
		- `Factor de escalado`: Todos los monitores | 100%
		- `Controlador gráfico`: VBoxVGA
		- Caracteristicas extendidas:
			- [ ] Habilitar aceleración 3D
	- Pantalla remota
		- [ ] Habilitar servidor
	- Grabación
		- [ ] Habilitar grabación

- Almacenamiento

- Audio
	- [ ] Habilitar audio

		- `Controlador de audio anfitrión`: Predeterminado
		- `Controlador de audio`: ICH 1C97
		- Características extendidas:
			- [X] Habilitar salida de audio
			- [ ] Habilitar entrada de audio

- Red
	- Adaptador 1
		- [ ] Habilitar adaptador de red
			- `Conectado a`: Adaptador puente
			- `Nombre`: ...
			- \> Avanzado
				- `Tipo de adaptador`: ...
				- `Modo promiscuo`: Denegar
				- `Dirección MAC`: ...
				- [X] Cable conectado
	- Adaptador 2
	- Adaptador 3
	- Adaptador 4

- Puertos serie
	- Puerto 1
		- [ ] Habilitar puerto serie
	- Puerto 2
	- Puerto 3
	- Puerto 4

- USB
	- [X] Habilitar controlador USB
		- (O) Controlador USB 1.1 (OHCI)
		- (O) Controlador USB 2.0 (OHCI + EHCI)
		- (O) Controlador USB 3.0 (xHCI)
		- Filtros de dispositivos USB

- Carpetas compartidas
	- v Carpetas de la máquina
		- `Nombre`: Compartida | `Ruta`: C:\Users\Usuario\VirtualBox VMs\Compartida | `Acceso`: Completo | `Automontar`: Sí | `En`:
	- v Carpetas transitorias

- Interfaz de usuario
	- [X] Archivo
	- [X] Máquina
	- [X] Ver
	- [X] Entrada
	- [X] Dispositivos
	- [X] Ayuda
	- [X] .
	- `Estado visual`: Normal (ventana)
	- Minibarra de herramientas:
		- [X] Mostrar en pantalla completa/fluído
		- [ ] Mostrar en la parte superior de la pantalla
	- [X] .
		- [X] .
		- [X] .
		- [X] .
		- [X] .
		- [X] .
		- [X] .
		- [X] .
		- [X] .
		- [X] .
		- [X] .
		- [X] .
		- [X] .

-----------------------------
## 9. Actualizar el índice de paquetes local en la VM

```bash
sudo apt update
```

-----------------------------

## Opcional: GRUB (para settear el tamaño de pantalla FIJO de la VM)

1. Backupear el archivo `/etc/default/grub` original
```bash
sudo cp /etc/default/grub /etc/default/grub.original
```

2. Editar el archivo `/etc/default/grub`
```bash
sudo vi /etc/default/grub
```

- Agregar al final la siguiente línea:
```output
GRUB_GFXPAYLOAD_LINUX=800x600
```

- Modificar la siguiente línea:
```output
GRUB_CMDLINE_LINUX_DEFAULT=""
```
a:
```output
GRUB_CMDLINE_LINUX_DEFAULT="nomodeset"
```

3. Actualizar el gestor de arranque GRUB

```bash
sudo update-grub
```

4. Reiniciar la VM
```bash
init 6
```

-----------------------------

## 10. Instalar VirtualBox Guest Additions (para las carpetas compartidas)

1. Iniciada la VM, ir a: `Dispositivos` > `Insertar imagen de CD de las Guest Additions`

2. Crear el directorio donde se montará el CD de las Guest Additions:
```bash
sudo mkdir /mnt/cdrom
```

3. Montar el CD de las Guest Additions en dicho directorio creado
```bash
sudo mount /dev/cdrom /mnt/cdrom
```

4. Ejecutar el instalador de las Guest Additions para Linux
```bash
sudo /mnt/cdrom/VBoxLinuxAdditions.run
```

5. Por si acaso, también agregar el user actual (`utnso`) al grupo de usuarios `vboxsf` para tener permisos de lectura y escritura en las carpetas compartidas:
```bash
sudo adduser $USER vboxsf
sudo usermod -aG vboxsf $USER
```

-----------------------------

## 11. Montar carpeta compartida de VirtualBox en la VM

1. Crear el directorio donde montaremos la carpeta compartida en la VM
```bash
mkdir /home/utnso/vboxsfCompartida
```

2. Montar manualmente la carpeta compartida en la VM
```bash
sudo mount -t vboxsf Compartida /home/utnso/vboxsfCompartida -o rw,exec,uid=1000,gid=1000
```

3. Verificar que la carpeta compartida se haya montado correctamente en la VM
```bash
ls /home/utnso/vboxsfCompartida
```

4. Para que la carpeta compartida se monte automáticamente cada vez que iniciemos la VM, editar el archivo `/etc/fstab`:
```bash
sudo vi /etc/fstab
```

Agregarle la siguiente línea al final de dicho archivo.
Nótese el uso de tabulaciones en lugar de espacios para separar las columnas de la línea.
```output
Compartida	/home/utnso/vboxsfCompartida	vboxsf	rw,exec,uid=1000,gid=1000	0	0
```

5. Para arrancar el servicio de carpetas compartidas de VirtualBox cada vez que iniciemos la VM, editar el archivo `/etc/modules`:
```bash
sudo vi /etc/modules
```

Agregarle la siguiente línea al final de dicho archivo.
```output
vboxsf
```

### Nota sobre las carpetas compartidas de VirtualBox

No andan los `Run and Debug` (`launch.json`) de VSCode para depurar los ejecutables si estos están dentro de las carpetas compartidas
Parece ser por una limitación de los permisos de ejecución del filesystem de VirtualBox (vboxsf)

-----------------------------

## Opcional. Montar una carpeta compartida usando Samba (SMB)

Este método no tiene la limitación anteriormente mencionada.

En la VM Ubuntu Server:

1. Descargar e instalar `cifs-utils`
```bash
sudo apt install cifs-utils
```

2. Crear el directorio donde montaremos la carpeta compartida en la VM
```bash
mkdir /home/utnso/smbCompartida
```

3. Montar manualmente la carpeta compartida en la VM
```bash
sudo mount -t cifs //NombreOIPDelHostWindows/NombreCarpetaCompartida /home/utnso/smbCompartida -o username=nombreUsuarioWindows,password=ContraseniaUsuarioWindows,vers=3.0,file_mode=0777,dir_mode=0777
```

4. Verificar que la carpeta compartida se haya montado correctamente en la VM
```bash
ls /home/utnso/smbCompartida
```

5. Para que la carpeta compartida se monte automáticamente cada vez que iniciemos la VM, editar el archivo `/etc/fstab`:
```bash
sudo vi /etc/fstab
```

Agregarle la siguiente línea al final de dicho archivo.
```output
//NombreOIPDelHostWindows/NombreCarpetaCompartida /home/utnso/smbCompartida cifs username=nombreUsuarioWindows,password=ContraseniaUsuarioWindows,vers=3.0,file_mode=0777,dir_mode=0777 0 0
```

Nota: `vers=3.0` es para indicar la versión de Samba (SMB) utilizada. Puede cambiarse a 2.0, por ejemplo, de ser necesario

-----------------------------

## 12. Configurar SSH en la VM

1. Descargar e instalar openssh-server
```bash
sudo apt install openssh-server -y
```

2. Asegurarse de que `ssh.service` esté activo (ejecutando)
```bash
sudo systemctl status ssh
sudo systemctl enable ssh --now
sudo systemctl status ssh
```

La salida debería ser similar a:
```output
● ssh.service - OpenBSD Secure Shell server
     Loaded: loaded (/lib/systemd/system/ssh.service; enabled; vendor preset: enabled)
     Active: active (running) since Wed yyyy-mm-dd hh:mm:ss UTC; #h ago
       Docs: man:sshd(8)
             man:sshd_config(5)
    Process: 733 ExecStartPre=/usr/sbin/sshd -t (code=exited, status=0/SUCCESS)
   Main PID: 778 (sshd)
      Tasks: 1 (limit: 2220)
     Memory: 7.7M
        CPU: 125ms
     CGroup: /system.slice/ssh.service
             └─778 "sshd: /usr/sbin/sshd -D [listener] 0 of 10-100 startups"
```

3. Asegurarse de que el firewall esté funcionando y de que permita ssh
```bash
sudo ufw status verbose
sudo ufw allow ssh
sudo ufw enable
sudo ufw status verbose
```

La salida debería ser algo así como:
```output
Status: active
Logging: on (low)
Default: deny (incoming), allow (outgoing), disabled (routed)
New profiles: skip

To                         Action      From
--                         ------      ----
22/tcp                     ALLOW IN    Anywhere                  
22/tcp (v6)                ALLOW IN    Anywhere (v6)
```

4. Asegurarse de que se escuchen los puertos de sshd
```bash
sudo lsof -i -P -n | grep LISTEN
```

La salida debería ser algo así como:
```output
systemd-r  697 systemd-resolve   14u  IPv4  19989      0t0  TCP 127.0.0.53:53 (LISTEN)
sshd       778            root    3u  IPv4  20570      0t0  TCP *:22 (LISTEN)
sshd       778            root    4u  IPv6  20624      0t0  TCP *:22 (LISTEN)
apache2    783            root    4u  IPv6  20598      0t0  TCP *:80 (LISTEN)
code-e170 1313           utnso    9u  IPv4  22229      0t0  TCP 127.0.0.1:39375 (LISTEN)
apache2   2816        www-data    4u  IPv6  20598      0t0  TCP *:80 (LISTEN)
apache2   2817        www-data    4u  IPv6  20598      0t0  TCP *:80 (LISTEN)
```

5. Editar el archivo de sshd_config
```bash
sudo vi /etc/ssh/sshd_config
```

Debajo de esta línea:
`Include /etc/ssh/sshd_config.d/*.conf`

Agregar las IPs admitidas para conectarse vía SSH con la VM.
Las que no figuren en este listado serán rechazadas, por más que ingresen usuario y contraseña correctos.
```console
AllowUsers utnso@NúmeroIP
```

6. Reiniciar el servicio de ssh
```bash
sudo systemctl restart ssh
```

7. Ver cuál es la IP de la VM
```bash
ifconfig
```

8. Ver el log: sirve para saber qué IPs intentaron conectarse vía SSH con la VM
Puede ser útil para averigüar la IP del Host
```bash
less /var/log/auth.log
```

-----------------------------

## 13. Conectarse por SSH a la VM

### En Windows (PowerShell):

1. Conocer la IP del Host
```powershell
ipconfig
```

2. Conectarse con la VM vía ssh
```powershell
ssh utnso@NúmeroIP -p 22
```
Nota: el puerto por lo general es 22

-----------------------------

## 14. Configurar VSCode

### Extensiones:
	Remote - SSH
	Remote - SSH: Editing Configuration Files
	Remote Explorer
	WSL
	C/C++
	C/C++ Extension Pack
	C/C++ Themes
	GitLens
	Live Share
	Makefile Tools
	Markdown All In One
	Markdown Preview Github Styling
	Notepad++ keymap
	Output Colorizer

### En Windows Host:

https://code.visualstudio.com/docs/remote/troubleshooting#_installing-a-supported-ssh-client
https://code.visualstudio.com/docs/remote/troubleshooting#_installing-a-supported-ssh-server

https://learn.microsoft.com/en-us/windows-server/administration/openssh/openssh_install_firstuse?tabs=powershell#tabpanel_1_powershell

1. To install OpenSSH using PowerShell, run PowerShell as an Administrator. To make sure that OpenSSH is available, run the following cmdlet:

```powershell
Get-WindowsCapability -Online | Where-Object Name -like 'OpenSSH*'
```
The command should return the following output if neither are already installed:

```output
Name  : OpenSSH.Client~~~~0.0.1.0
State : NotPresent

Name  : OpenSSH.Server~~~~0.0.1.0
State : NotPresent
```

2. Then, install the server or client components as needed:

```powershell
# Install the OpenSSH Client
Add-WindowsCapability -Online -Name OpenSSH.Client~~~~0.0.1.0

# Install the OpenSSH Server
Add-WindowsCapability -Online -Name OpenSSH.Server~~~~0.0.1.0
```
Both commands should return the following output:

```output
Path          :
Online        : True
RestartNeeded : False
```
3. To start and configure OpenSSH Server for initial use, open an elevated PowerShell prompt (right click, Run as an administrator), then run the following commands to start the sshd service:

```powershell
# Start the sshd service
Start-Service sshd

# OPTIONAL but recommended:
Set-Service -Name sshd -StartupType 'Automatic'

# Confirm the Firewall rule is configured. It should be created automatically by setup. Run the following to verify
if (!(Get-NetFirewallRule -Name "OpenSSH-Server-In-TCP" -ErrorAction SilentlyContinue | Select-Object Name, Enabled)) {
    Write-Output "Firewall Rule 'OpenSSH-Server-In-TCP' does not exist, creating it..."
    New-NetFirewallRule -Name 'OpenSSH-Server-In-TCP' -DisplayName 'OpenSSH Server (sshd)' -Enabled True -Direction Inbound -Protocol TCP -Action Allow -LocalPort 22
} else {
    Write-Output "Firewall rule 'OpenSSH-Server-In-TCP' has been created and exists."
}
```

- https://code.visualstudio.com/docs/remote/ssh-tutorial

4. Presionar el botón azul en la esquina inferior izquierda de VSCode

5. Seleccionar la opción `Connect to Host...`

6. Ingresar en el siguiente formato:
`utnso@NúmeroIP`

7. Guardar la configuración en:
`C:\Users\Usuario\.ssh\config`

8. Verificar el archivo anterior:
```output
# Nombre
Host NúmeroIP
  HostName NúmeroIP
  User utnso
  Port 22
```

-----------------------------

## 15. Configurar Git en la VM

1. Generar un token en GitHub
	- Tokens (classic)
	https://github.com/settings/tokens
	The minimum required scopes are 'repo', 'read:org', 'workflow'.

	- Fine-grained tokens
	https://github.com/settings/tokens?type=beta

2. Instalar `gh` (no viene instalado en la VM de Ubuntu Server)

```bash
sudo apt install gh -y
```

3. Loggearse

```bash
gh auth login
```

4. Hacer los git config

```bash
git config --global user.email "ejemplo@frba.utn.edu.ar"
git config --global user.name "FDM"
```

-----------------------------

## 16. Instalar la versión más reciente de CMake

https://cmake.org/download/

wget

https://github.com/Kitware/CMake/releases/download/v3.29.0/cmake-3.29.0-linux-x86_64.sh

```bash
chmod +x cmake-3.29.0-linux-x86_64.sh
sudo ./cmake-3.29.0-linux-x86_64.sh --prefix=/usr/local --skip-license
```

-----------------------------

## Anexo 1: Comandos útiles

> Cambiar a root
```bash
sudo -s
```

> Salir como root
```bash
exit
```

> Desmontar un disco
```bash
sudo umount /ruta/al/punto/de/montaje
```

> Leer un archivo de texto
```bash
less
```

> Editar un archivo de texto
```bash
nano
vi
```

> Listar configuración de red
```bash
ifconfig
```

> Apagar
```bash
shutdown now
```

> Reiniciar
```bash
shutdown -r now
```

> Listar procesos
```bash
htop
```

-----------------------------

## Anexo 2: tmux (Terminal MUltipleXer)

Abrir una sesión nueva
```bash
tmux
```
Equivalente:
```bash
tmux new
```
Abrir una sesión nueva y ponerle un nombre:
```bash
tmux new -s 'NombreDeSesion'
```

Cerrar la ventana actual de una sesión
```bash
exit
```

- Para cerrar forzosamente la ventana de la sesion, presione <kbd>Ctrl</kbd> + <kbd>b</kbd>, seguidamente presione <kbd>x</kbd> y por ultimo presione <kbd>y</kbd>
- Para crear una nueva ventana, presione <kbd>Ctrl</kbd> + <kbd>b</kbd> y seguidamente presione <kbd>c</kbd>
- Para apartar la sesion con sus ventanas sin cerrarla [detach], presione <kbd>Ctrl</kbd> + <kbd>b</kbd> y seguidamente presione <kbd>d</kbd>
- Para volver a las sesiones apartadas de tmux [detached], ejecute el comando:
```bash
tmux attach
```
	- Por el número identificador de la sesión
```bash
tmux attach -t 0
```
	- Por el nombre de la sesión
```bash
tmux attach -t 'NombreDeSesion'
```
- Para listar cuántas sesiones de tmux están abiertas, ejecute el comando:
```bash
tmux ls
```
- Para cerrar forzosamente una sesión de tmux, ejecute el comando:
```bash
tmux kill-session
```
	- Por el número identificador de la sesión
```bash
tmux kill -t 0
```
	- Por el nombre de la sesión
```bash
tmux kill -t 'NombreDeSesion'
```
- Para cambiarle el nombre a la sesion actual de tmux, presione <kbd>Ctrl</kbd> + <kbd>b</kbd> y seguidamente presione <kbd>,</kbd>
- Para alternar entre las sesiones abiertas de tmux, presione <kbd>Ctrl</kbd> + <kbd>b</kbd> y seguidamente presione <kbd>s</kbd>
- Para alternar entre las ventanas de las sesiones abiertas de tmux, presione <kbd>Ctrl</kbd> + <kbd>b</kbd> y seguidamente presione <kbd>w</kbd>
- Para iniciar el modo desplazamiento por la ventana, presione <kbd>Ctrl</kbd> + <kbd>b</kbd> y seguidamente presione <kbd>[</kbd> (con la distribucion de teclado latinoamericano, <kbd>[</kbd> es <kbd>⇧ Shift</kbd> + <kbd>{</kbd>)
- Para finalizar el modo desplazamiento por la ventana, presione <kbd>q</kbd>

-----------------------------

## Anexo 3: Opciones importantes de gcc

`-DDEBUG -fdiagnostics-color=always -lcommons -lpthread -lreadline -lm`

-----------------------------

# Al salir

## 1. Desloguearse de Git, sacar las credenciales de Windows