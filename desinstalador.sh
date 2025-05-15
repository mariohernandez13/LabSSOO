#!/bin/bash

# 🔧 CONFIGURACIÓN
USUARIO="appuser"                             # ← Cambia esto si usas otro usuario del sistema
RUTA_APP="/opt/securebank"                    # ← Cambia esto si mueves la app a otro directorio
BINARIO="init_cuentas"                        # ← Cambia esto si tu binario principal tiene otro nombre
SERVICE_FILE="/etc/systemd/system/securebank.service"  # ← Cambia si usas otro nombre de servicio

echo "⚠️ Iniciando desinstalación de SecureBank..."

# 1. Detener el servicio si está activo
if systemctl is-active --quiet securebank.service; then
    echo "⏹️ Deteniendo servicio..."
    sudo systemctl stop securebank.service
fi

# 2. Deshabilitar el servicio
if systemctl is-enabled --quiet securebank.service; then
    echo "🚫 Deshabilitando servicio..."
    sudo systemctl disable securebank.service
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
echo "   Verifica con: sudo systemctl status securebank.service"