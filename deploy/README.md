# Notas de Deploy SSOO

Nota: algunas de las cosas que se listan sirven para poder trabajar sobre el repositorio de manera cómoda, no sólo sirve para el deploy

## TODO
- Revisar permisos necesarios de administrador en Windows: en las máquinas de la facultad no tenemos :(
- Evaluar cómo hacer para pasar el repositorio y los scripts de deploy lo más fácil posible a las máquinas de la facultad
- Hacer un script/comando que habilite/deshabilite características de Windows
- Hacer un script que configure automáticamente la VM en VirtualBox, o llevar los archivos de configuración ya hechos
- Hacer un script que instale automáticamente las extensiones de VSCode
- Hacer un script que tome las IPs del Host y del Guest y que haga las conexiones SSH automáticamente y edite los .config acordemente
- Editar el archivo /home/utnso/base.sh acordemente

-----------------------------

## 1. Características de Windows

- Desactivar Plataforma de Hipervisor de Windows

-----------------------------

## 2. Configuración de la VM (Ubuntu Server) en VirtualBox

General
	Básico
		Nombre: Ubuntu Server 6.1
		Tipo: Linux
		Versión: Ubuntu (64-bit)
	Avanzado
		Carpeta de instantáneas: C:\Users\Usuario\VirtualBox VMs\Ubuntu Server 6.1\Snapshots
		Portapapeles compartido: Inhabilitado
		Arrastrar y soltar: Inhabilitado
	Descripción
	Cifrado de disco
		[ ] Habilitar cifrado de disco

Sistema
	Placa base
		Memoria base: 2048 MB
		Orden de arranque: Óptica - Disco Duro
		Chipset: PIIX3
		TPM: Ninguno
		Dispositivo apuntador: Tableta USB
		Características extendidas:
			[X] Habilitar I/O APIC
			[X] Habilitar reloj hardware en tiempo UTC
			[ ] Habilitar EFI (sólo SO especiales)
			[ ] Habilitar Secure Boot
	Procesador
		Procesadores: 1
		Límite de ejecución: 100%
		Características extendidas:
			[ ] Habilitar PAE/NX
			[ ] Habilitar VT-x/AMD-V anidado
	Aceleración
		Interfaz de paravirtualización: Predeterminado
		Hardware de virtualización:
			[X] Habilitar paginación anidada

Pantalla
	Pantalla
		Memoria de video: 16 MB
		Número de monitores: 1
		Factor de escalado: Todos los monitores | 100%
		Controlador gráfico: VBoxVGA
		Caracteristicas extendidas:
			[ ] Habilitar aceleración 3D
	Pantalla remota
		[ ] Habilitar servidor
	Grabación
		[ ] Habilitar grabación

Almacenamiento

Audio
	[ ] Habilitar audio

		Controlador de audio anfitrión: Predeterminado
		Controlador de audio: ICH 1C97
		Características extendidas:
			[X] Habilitar salida de audio
			[ ] Habilitar entrada de audio

Red
	Adaptador 1
		[ ] Habilitar adaptador de red
			Conectado a: Adaptador puente
			Nombre: Intel(R) Dual Band Wireless-AC 3165
			> Avanzado
				Tipo de adaptador: ...
				Modo promiscuo: Denegar
				Dirección MAC: ...
				[X] Cable conectado
	Adaptador 2
	Adaptador 3
	Adaptador 4

Puertos serie
	Puerto 1
		[ ] Habilitar puerto serie
	Puerto 2
	Puerto 3
	Puerto 4

USB
	[X] Habilitar controlador USB
		(O) Controlador USB 1.1 (OHCI)
		(O) Controlador USB 2.0 (OHCI + EHCI)
		(O) Controlador USB 3.0 (xHCI)
		Filtros de dispositivos USB

Carpetas compartidas
	v Carpetas de la máquina
		Nombre: Compartida | Ruta: C:\Users\Usuario\VirtualBox VMs\Compartida | Acceso: Completo | Automontar: Sí | En:
	v Carpetas transitorias

Interfaz de usuario
	[X] Archivo
	[X] Máquina
	[X] Ver
	[X] Entrada
	[X] Dispositivos
	[X] Ayuda
	[X]
	Estado visual: Normal (ventana)
	Minibarra de herramientas:
		[X] Mostrar en pantalla completa/fluído
		[ ] Mostrar en la parte superior de la pantalla
	[X]
		[X]
		[X]
		[X]
		[X]
		[X]
		[X]
		[X]
		[X]
		[X]
		[X]
		[X]
		[X]

-----------------------------

## Anexo 1: Comandos útiles

Acceder como root
```bash
sudo -s
```
Leer un archivo de texto
```bash
less
```
Editar un archivo de texto
```bash
nano
vi
```
Listar configuración de red
```bash
ifconfig
```
Apagar
```bash
shutdown now
```
Reiniciar
```bash
shutdown -r now
```
Listar procesos
```bash
htop
```

-----------------------------

## Anexo 2: Abrir una sesión de tmux (Terminal MUltipleXer)

```bash
tmux
```

- Para cerrar la ventana de la sesion, presione <kbd>Ctrl</kbd> + <kbd>b</kbd>, seguidamente presione <kbd>x</kbd> y por ultimo presione <kbd>y</kbd>
- Para apartar la sesion con sus ventanas sin cerrarla [detach], presione <kbd>Ctrl</kbd> + <kbd>b</kbd> y seguidamente presione <kbd>d</kbd>
- Para alternar entre las sesiones abiertas de tmux, presione <kbd>Ctrl</kbd> + <kbd>b</kbd> y seguidamente presione <kbd>s</kbd>
- Para alternar entre las ventanas de las sesiones abiertas de tmux, presione <kbd>Ctrl</kbd> + <kbd>b</kbd> y seguidamente presione <kbd>w</kbd>
- Para iniciar el modo desplazamiento por la ventana, presione <kbd>Ctrl</kbd> + <kbd>b</kbd> y seguidamente presione <kbd>[</kbd> (con la distribucion de teclado latinoamericano, <kbd>[</kbd> es <kbd>⇧ Shift</kbd> + <kbd>{</kbd>)
- Para finalizar el modo desplazamiento por la ventana, presione <kbd>q</kbd>

NOTA: Para volver a las sesiones apartadas de tmux [detached], ejecute el comando:

```bash
tmux attach
```

-----------------------------
## Anexo 3: Opciones importantes de gcc

`-DDEBUG -fdiagnostics-color=always -lcommons -lpthread -lreadline -lm`

-----------------------------
## 3. Actualizar el índice de paquetes local en la VM

```bash
sudo apt update
```

-----------------------------

## Opcional: GRUB (para cambiar el tamaño de pantalla de la VM)

```bash
sudo cp /etc/default/grub /etc/default/grub.original
sudo printf '%s\n' 'GRUB_GFXPAYLOAD_LINUX=800x600' >> /etc/default/grub
```

CAMBIAR la línea:
```bash
GRUB_CMDLINE_LINUX_DEFAULT=""
```
a
```bash
GRUB_CMDLINE_LINUX_DEFAULT="nomodeset"
```

```bash
sudo update-grub
init 6
```

-----------------------------

## 4. Instalar VirtualBox Guest Additions (para las carpetas compartidas)

1. Iniciada la VM, ir a: `Dispositivos` > `Insertar imagen de CD de las Guest Additions`

2. Ejecutar los siguientes comandos:

```bash
sudo mkdir /mnt/cdrom
sudo mount /dev/cdrom /mnt/cdrom
sudo /mnt/cdrom/VBoxLinuxAdditions.run
```

Por las dudas también:

```bash
sudo adduser $USER vboxsf
sudo usermod -aG vboxsf $USER
```

-----------------------------

## 5. Configurar carpeta compartida

### Nota sobre las carpetas compartidas de VirtualBox

No andan las Launch Tasks (launch.json) de VSCode para depurar los ejecutables si estos están dentro de las carpetas compartidas
Parece ser por una limitación de los permisos de ejecución del filesystem de VirtualBox (vboxfs)

-----------------------------

## 6. Configurar SSH en la VM

```bash
sudo apt-get install openssh-server -y
```

```bash
sudo systemctl status ssh
sudo systemctl enable ssh --now
sudo systemctl status ssh
```

```bash
sudo ufw status
sudo ufw allow ssh
sudo ufw enable
sudo ufw status verbose
```

```bash
sudo lsof -i -P n | grep LISTEN
```

```bash
sudo vi /etc/ssh/sshd_config
```

```bash
sudo systemctl restart ssh
```

```bash
ifconfig
```

Log (para ver qué IPs intentaron conectarse vía SSH con la VM)
```bash
less /var/log/auth.log
```

-----------------------------

## 7. Conectarse por SSH a la VM

En Windows (PowerShell):

```powershell
ipconfig
```

```powershell
ssh utnso@IP -p 22
```

-----------------------------

## 8. Configurar VSCode

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

PowerShell (como administrador)

To install OpenSSH using PowerShell, run PowerShell as an Administrator. To make sure that OpenSSH is available, run the following cmdlet:

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
Then, install the server or client components as needed:

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
To start and configure OpenSSH Server for initial use, open an elevated PowerShell prompt (right click, Run as an administrator), then run the following commands to start the sshd service:

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

utnso@IP

Guardar la configuración en:

C:\Users\Usuario\.ssh\config

```output
# Nombre
Host IP
  HostName IP
  User utnso
  Port 22
```

-----------------------------

## Configurar Git en la VM

1. Pedir un token de GitHub
	Tokens (classic)
	https://github.com/settings/tokens
	The minimum required scopes are 'repo', 'read:org', 'workflow'.

	Fine-grained tokens
	https://github.com/settings/tokens?type=beta

2. Instalar gh (no viene instalado en la VM de Ubuntu Server)

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

## Instalar la versión más reciente de CMake

https://cmake.org/download/

wget

https://github.com/Kitware/CMake/releases/download/v3.29.0/cmake-3.29.0-linux-x86_64.sh

```bash
chmod +x cmake-3.29.0-linux-x86_64.sh
sudo ./cmake-3.29.0-linux-x86_64.sh --prefix=/usr/local --skip-license
```