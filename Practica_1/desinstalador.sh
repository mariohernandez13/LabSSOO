#!/bin/bash

# 🔧 CONFIGURACIÓN
USUARIO="appuser"                             # ← Cambia esto si usas otro usuario del sistema
RUTA_APP="/opt/securebank"                    # ← Cambia esto si mueves la app a otro directorio
BINARIO="init_cuentas"                        # ← Cambia esto si tu binario principal tiene otro nombre
SERVICE_NAME="securebank.service"             # ← Cambia si usas otro nombre de servicio
SERVICE_FILE="/etc/systemd/system/$SERVICE_NAME"
LOG="/tmp/securebank_uninstall.log"           # Puedes cambiarlo a /var/log/... si lo prefieres

echo "⚠️ Iniciando desinstalación de SecureBank..."
echo "$(date '+%F %T') ⚠️ Desinstalación iniciada" | sudo tee -a "$LOG" > /dev/null

# 1. Detener el servicio si está activo
if systemctl is-active --quiet "$SERVICE_NAME"; then
    echo "⏹️ Deteniendo servicio..."
    sudo systemctl stop "$SERVICE_NAME"
fi

# 2. Deshabilitar el servicio
if systemctl is-enabled --quiet "$SERVICE_NAME"; then
    echo "🚫 Deshabilitando servicio..."
    sudo systemctl disable "$SERVICE_NAME"
fi

# 3. Eliminar el archivo de servicio
if [ -f "$SERVICE_FILE" ]; then
    echo "🗑️ Eliminando archivo de servicio: $SERVICE_FILE"
    sudo rm "$SERVICE_FILE"
else
    echo "ℹ️ No se encontró el archivo de servicio"
fi

# 4. Eliminar los archivos de la app
if [ -d "$RUTA_APP" ]; then
    echo "🗑️ Eliminando archivos de la aplicación en $RUTA_APP"
    sudo rm -rf "$RUTA_APP"
else
    echo "ℹ️ No se encontró el directorio de la aplicación"
fi

# 5. Eliminar el usuario del sistema
if id "$USUARIO" &>/dev/null; then
    echo "🧹 Eliminando usuario del sistema: $USUARIO"
    sudo deluser --system "$USUARIO"
else
    echo "ℹ️ El usuario $USUARIO no existe"
fi

# 6. Recargar systemd
echo "🔄 Recargando systemd..."
sudo systemctl daemon-reload

echo "✅ Desinstalación completada."
echo "$(date '+%F %T') ✅ Desinstalación completada" | sudo tee -a "$LOG" > /dev/null
echo "   Verifica con: sudo systemctl status $SERVICE_NAME"
