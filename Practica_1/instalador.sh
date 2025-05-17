#!/bin/bash

###############################################################################
# 🛠️ Script de instalación de entorno SecureBank
# 
# Funciones:
#   - Crear un nuevo usuario
#   - Copiar el programa al directorio del usuario
#   - Crear acceso directo en el escritorio
#   - Permitir ejecución con sudo sin contraseña
###############################################################################

# ─── CONFIGURACIÓN PERSONALIZABLE ────────────────────────────────────────────
NUEVO_USUARIO="SecureBank"
CLAVE="banco"
RUTA_ORIG="./"
RUTA_SCRIPT="instalador.sh"
NOMBRE_PROGRAMA="init_cuentas"
# ─────────────────────────────────────────────────────────────────────────────

# Variables derivadas
RUTA_DEST="/home/$NUEVO_USUARIO/securebank"
DESKTOP_DIR="/home/$NUEVO_USUARIO/Desktop"
LAUNCHER="$DESKTOP_DIR/$NOMBRE_PROGRAMA.desktop"
SCRIPT_COMPLETO="$RUTA_DEST/$RUTA_SCRIPT"

# ─── 1. Crear nuevo usuario ──────────────────────────────────────────────────
echo -e "\n🔧 [1/5] Creando usuario '\e[1m$NUEVO_USUARIO\e[0m'..."
sudo useradd -m "$NUEVO_USUARIO"
echo "$NUEVO_USUARIO:$CLAVE" | sudo chpasswd

# ─── 2. Copiar programa ──────────────────────────────────────────────────────
echo -e "📂 [2/5] Copiando programa a \e[1m$RUTA_DEST\e[0m..."
sudo cp -r "$RUTA_ORIG" "$RUTA_DEST"
sudo chown -R "$NUEVO_USUARIO:$NUEVO_USUARIO" "$RUTA_DEST"
sudo chmod -R u+rwX "$RUTA_DEST"

# ─── 3. Crear acceso directo ─────────────────────────────────────────────────
echo -e "📎 [3/5] Creando acceso directo en \e[1m$DESKTOP_DIR\e[0m..."
sudo mkdir -p "$DESKTOP_DIR"

sudo bash -c "cat > '$LAUNCHER'" <<EOF
[Desktop Entry]
Type=Application
Name=$NOMBRE_PROGRAMA
Exec=gnome-terminal -- bash -c 'cd $RUTA_DEST && ./init_cuentas ; exec bash'
Icon=utilities-terminal
Terminal=false
EOF

sudo chmod +x "$LAUNCHER"
sudo chown "$NUEVO_USUARIO:$NUEVO_USUARIO" "$LAUNCHER"

# ─── 4. Configurar sudo sin contraseña ───────────────────────────────────────
echo -e "🔐 [4/5] Configurando sudoers para ejecución sin contraseña..."
SUDOERS_LINE="$NUEVO_USUARIO ALL=(ALL) NOPASSWD: $SCRIPT_COMPLETO"
echo "$SUDOERS_LINE" | sudo tee "/etc/sudoers.d/$NUEVO_USUARIO-programa" > /dev/null
sudo chmod 440 "/etc/sudoers.d/$NUEVO_USUARIO-programa"

# ─── 5. Final ────────────────────────────────────────────────────────────────
echo -e "\n✅ \e[1mCONFIGURACIÓN COMPLETA\e[0m"
echo -e "═══════════════════════════════════════════════════════════════"
echo "👤 Usuario:           $NUEVO_USUARIO"
echo "🔑 Contraseña:        $CLAVE"
echo "📁 Programa en:       $RUTA_DEST"
echo "🖥️  Acceso directo:   $LAUNCHER"
echo ""
echo -e "📌 \e[1mPASOS FINALES\e[0m:"
echo "1. Cierra tu sesión actual (logout)"
echo "2. Inicia sesión como '$NUEVO_USUARIO'"
echo "3. Ingresa la contraseña."
echo "4. Haz doble clic en el icono '$NOMBRE_PROGRAMA' del escritorio"
echo "   Se abrirá GNOME Terminal y ejecutará el programa."
echo "═══════════════════════════════════════════════════════════════"
