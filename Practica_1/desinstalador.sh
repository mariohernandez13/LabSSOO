#!/bin/bash

###############################################################################
# ❌ Desinstalador de SecureBank
#
# Elimina:
#   - El usuario creado (SecureBank)
#   - Archivos de programa
#   - Acceso directo en el escritorio
#   - Reglas sudo personalizadas
###############################################################################

# ─── CONFIGURACIÓN ───────────────────────────────────────────────────────────
NUEVO_USUARIO="SecureBank"
NOMBRE_PROGRAMA="init_cuentas"
RUTA_DEST="/home/$NUEVO_USUARIO/securebank"
DESKTOP_DIR="/home/$NUEVO_USUARIO/Desktop"
LAUNCHER="$DESKTOP_DIR/$NOMBRE_PROGRAMA.desktop"
SUDOERS_FILE="/etc/sudoers.d/$NUEVO_USUARIO-programa"
# ─────────────────────────────────────────────────────────────────────────────

echo -e "\n🧹 \e[1mINICIANDO DESINSTALACIÓN DE SECUREBANK\e[0m"
echo "═══════════════════════════════════════════════════════════════"

# ─── 1. Eliminar el acceso directo ──────────────────────────────────────────
if [ -f "$LAUNCHER" ]; then
    echo "🗑️  Eliminando acceso directo: $LAUNCHER"
    sudo rm -f "$LAUNCHER"
else
    echo "ℹ️  No se encontró acceso directo en el escritorio."
fi

# ─── 2. Eliminar carpeta del programa ───────────────────────────────────────
if [ -d "$RUTA_DEST" ]; then
    echo "🗑️  Eliminando carpeta de aplicación: $RUTA_DEST"
    sudo rm -rf "$RUTA_DEST"
else
    echo "ℹ️  No se encontró la carpeta del programa."
fi

# ─── 3. Eliminar archivo sudoers ────────────────────────────────────────────
if [ -f "$SUDOERS_FILE" ]; then
    echo "🗑️  Eliminando entrada sudoers: $SUDOERS_FILE"
    sudo rm -f "$SUDOERS_FILE"
else
    echo "ℹ️  No se encontró configuración sudo personalizada."
fi

# ─── 4. Eliminar usuario y su home ──────────────────────────────────────────
if id "$NUEVO_USUARIO" &>/dev/null; then
    echo "👤 Eliminando usuario y su directorio home..."
    sudo userdel -r "$NUEVO_USUARIO"
else
    echo "ℹ️  El usuario '$NUEVO_USUARIO' no existe."
fi

# ─── 5. Final ───────────────────────────────────────────────────────────────
echo -e "\n✅ \e[1mDESINSTALACIÓN COMPLETADA\e[0m"
echo "═══════════════════════════════════════════════════════════════"
echo "Se han eliminado el usuario, la aplicación y los accesos."
echo ""