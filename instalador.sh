#!/bin/bash

# Variables de configuración
USUARIO="appuser"
RUTA_APP="/opt/securebank"
BINARIO="init_cuentas"
RUTA_BIN="$RUTA_APP/$BINARIO"
SERVICE_FILE="/etc/systemd/system/securebank.service"

echo "🔧 Creando usuario del sistema..."
if id "$USUARIO" &>/dev/null; then
    echo "ℹ️ Usuario $USUARIO ya existe. Continuando..."
else
    sudo adduser --system --no-create-home --group $USUARIO
fi

echo "📁 Creando directorio de aplicación..."
sudo mkdir -p "$RUTA_APP"

echo "📦 Copiando binario $BINARIO a $RUTA_APP..."
if [[ -f "./$BINARIO" ]]; then
    sudo cp "./$BINARIO" "$RUTA_BIN"
    sudo chown $USUARIO:$USUARIO "$RUTA_BIN"
    sudo chmod 750 "$RUTA_BIN"
else
    echo "❌ Error: el binario './$BINARIO' no existe."
    exit 1
fi

echo "📝 Creando servicio systemd..."
sudo bash -c "cat > $SERVICE_FILE" <<EOF
[Unit]
Description=SecureBank principal ejecutado por $USUARIO
After=network.target

[Service]
Type=simple
User=$USUARIO
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

echo "✅ Instalación completada."
echo "   Verifica con: sudo systemctl status securebank.service"

# dar permisos al sh
chmod +x instalador.sh
./instalador.sh

# verificar estado
systemctl status securebank.service
journalctl -u securebank.service
