#!/bin/bash

# Variables de configuración
USUARIO="appuser"
HOME_USUARIO="/home/$USUARIO"
RUTA_ORIGEN="$(cd "$(dirname "$0")" && pwd)"
RUTA_APP="/opt/securebank"
BINARIO="init_cuentas"
RUTA_BIN="$RUTA_APP/$BINARIO"
SERVICE_FILE="/etc/systemd/system/securebank.service"

echo "🔧 Creando usuario del sistema..."
if id "$USUARIO" &>/dev/null; then
    echo "ℹ️ Usuario $USUARIO ya existe. Continuando..."
else
    sudo adduser --home "$HOME_USUARIO" --shell /bin/bash --gecos "" "$USUARIO"
    sudo mkdir -p "$HOME_USUARIO"
    sudo cp /etc/skel/.bashrc "$HOME_USUARIO/"
    sudo chown -R "$USUARIO:$USUARIO" "$HOME_USUARIO"
    sudo chmod 700 "$HOME_USUARIO"
    echo "⚠️ Recuerda asignar contraseña si quieres que pueda iniciar sesión:"
    echo "   sudo passwd $USUARIO"
fi

echo "📁 Copiando archivos del proyecto desde $RUTA_ORIGEN a $RUTA_APP..."
sudo mkdir -p "$RUTA_APP"
sudo rsync -av --exclude="instalador_securebank.sh" "$RUTA_ORIGEN/" "$RUTA_APP/"

echo "🔐 Asignando permisos a la app..."
sudo chown -R "$USUARIO:$USUARIO" "$RUTA_APP"
sudo chmod -R 750 "$RUTA_APP"

# Variables para la sesión gráfica (asumimos UID 1000)
GRAPHICAL_UID=1000
# Usa el XAUTHORITY del usuario gráfico principal
XAUTHORITY="/run/user/$GRAPHICAL_UID/gdm/Xauthority"
# Si no existe, usa el .Xauthority del home del usuario gráfico
if [ ! -f "$XAUTHORITY" ]; then
    XAUTHORITY="/home/$(id -nu $GRAPHICAL_UID)/.Xauthority"
fi
DBUS_SESSION_BUS_ADDRESS="unix:path=/run/user/$GRAPHICAL_UID/bus"
AT_SPI_BUS_ADDRESS="unix:path=/run/user/$GRAPHICAL_UID/at-spi/bus"

echo "🔓 Ajustando permisos para que $USUARIO pueda acceder a la sesión gráfica..."
sudo setfacl -m u:$USUARIO:r "$XAUTHORITY" || echo "⚠️ Falló setfacl en XAUTHORITY"
sudo setfacl -m u:$USUARIO:r "/run/user/$GRAPHICAL_UID" || echo "⚠️ Falló setfacl en /run/user/$GRAPHICAL_UID"
sudo setfacl -m u:$USUARIO:r "/run/user/$GRAPHICAL_UID/bus" || echo "⚠️ Falló setfacl en bus"
sudo setfacl -m u:$USUARIO:r "/run/user/$GRAPHICAL_UID/at-spi/bus" || echo "⚠️ Falló setfacl en at-spi bus"

echo "📝 Creando servicio systemd con entorno gráfico para $USUARIO..."
sudo bash -c "cat > $SERVICE_FILE" <<EOF
[Unit]
Description=SecureBank principal ejecutado por $USUARIO
After=network.target

[Service]
Type=simple
User=$USUARIO
Environment=DISPLAY=:0
Environment=XAUTHORITY=$XAUTHORITY
Environment=DBUS_SESSION_BUS_ADDRESS=$DBUS_SESSION_BUS_ADDRESS
Environment=AT_SPI_BUS_ADDRESS=$AT_SPI_BUS_ADDRESS
WorkingDirectory=$RUTA_APP
ExecStart=$RUTA_BIN
Restart=on-failure

[Install]
WantedBy=multi-user.target
EOF

echo "🔄 Recargando systemd y activando el servicio..."
sudo systemctl daemon-reexec
sudo systemctl daemon-reload
sudo systemctl enable --now securebank.service

echo ""
echo "✅ Instalación completada."
echo ""
echo "🧑‍💻 Para ejecutar la app con interfaz gráfica:"
echo "   1. Abre una terminal gráfica (gnome-terminal) como tu usuario normal."
echo "   2. Ejecuta: sudo -iu $USUARIO"
echo "   3. Desde esa sesión, ejecuta: cd $RUTA_APP && ./init_cuentas"
echo ""
echo "📋 Puedes verificar el estado del servicio con:"
echo "   sudo systemctl status securebank.service"
echo "📄 Ver logs del servicio con:"
echo "   journalctl -u securebank.service"


# dar permisos al sh
# chmod +x instalador.sh
# ./instalador.sh

# verificar estado
# systemctl status securebank.service
# journalctl -u securebank.service
