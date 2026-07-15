import os
import shutil
import glob
import cantools
from cantools.database.can.c_source import generate

# ==========================================
# CONFIGURACIÓN DE RUTAS
# ==========================================
# "." significa "busca en la misma carpeta donde está este script"
DBC_FOLDER = "." 

# Carpetas destino relativas a donde está el script
INC_TARGET_DIR = os.path.join('Core', 'Inc')
SRC_TARGET_DIR = os.path.join('Core', 'Src')

def sync_dbc_to_stm32():
    try:
        # 1. Buscar archivos .dbc en el directorio actual
        dbc_files = glob.glob(os.path.join(DBC_FOLDER, "*.dbc"))
        
        # Extraer el primer elemento. 
        # Si la lista está vacía (no hay .dbc), provocará el IndexError.
        dbc_path = dbc_files[0]
        
        dbc_filename = os.path.basename(dbc_path)
        base_name = os.path.splitext(dbc_filename)[0]
        
        print(f"📦 Archivo DBC detectado: {dbc_filename}")
        print("🔄 Cargando base de datos CAN en memoria...")

        # 2. Cargar el archivo DBC usando la API nativa de cantools
        db = cantools.database.load_file(dbc_path)
        
        print("🛠️ Generando código fuente C y H...")
        
        # 👈 CAMBIO AQUÍ: Definimos los nombres ANTES de llamar a generate()
        generated_h = f"{base_name}.h"
        generated_c = f"{base_name}.c"
        fuzzer_c = f"{base_name}_fuzzer.c" # Obligatorio para la función, aunque no lo usemos

        # 3. Utilizar el generador de código pasándole los argumentos obligatorios
        header, source, f_header, f_source = generate(
            database=db,
            database_name=base_name,
            header_name=generated_h,         # 👈 Añadido
            source_name=generated_c,         # 👈 Añadido
            fuzzer_source_name=fuzzer_c,     # 👈 Añadido
            use_float=True  # Optimizado para microcontroladores STM32 con FPU
        )

        # 4. Escribir los contenidos generados en archivos temporales locales
        with open(generated_h, "w", encoding="utf-8") as f:
            f.write(header)
            
        with open(generated_c, "w", encoding="utf-8") as f:
            f.write(source)

        # 5. Asegurar la existencia de las carpetas destino en el proyecto STM32
        os.makedirs(INC_TARGET_DIR, exist_ok=True)
        os.makedirs(SRC_TARGET_DIR, exist_ok=True)

        # 6. Mover el archivo .h a Core/Inc
        dest_h = os.path.join(INC_TARGET_DIR, generated_h)
        if os.path.exists(dest_h):
            os.remove(dest_h)
        shutil.move(generated_h, dest_h)
        print(f"✔️ Cabecera guardada en: {dest_h}")

        # 7. Mover el archivo .c a Core/Src
        dest_c = os.path.join(SRC_TARGET_DIR, generated_c)
        if os.path.exists(dest_c):
            os.remove(dest_c)
        shutil.move(generated_c, dest_c)
        print(f"✔️ Fuente guardado en: {dest_c}")
        
        print("🚀 ¡Sincronización nativa completada con éxito!")

    except IndexError:
        print("❌ Error: No se encontró ningún archivo .dbc en la carpeta donde se ejecuta el script.")
    
    except (cantools.database.errors.ParseError, ValueError) as e:
        print(f"❌ Error de Sintaxis en DBC: Cantools no pudo procesar el archivo. Detalle: {e}")
        print("💡 Revisa el archivo .dbc con un editor de texto o una herramienta de diseño CAN.")
    
    except FileNotFoundError as e:
        print(f"❌ Error de Sistema: No se encontró un archivo o directorio intermedio. {e}")
    
    except PermissionError:
        print("❌ Error de Permisos: STM32CubeIDE o el sistema bloquean los archivos. Cierra los editores si están trabados.")
    
    except Exception as e:
        print(f"❌ Ocurrió un error inesperado de tipo [{type(e).__name__}]: {e}")

if __name__ == "__main__":
    sync_dbc_to_stm32()