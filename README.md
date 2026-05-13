# LiteDir

> Alternativa ligera a Active Directory para entornos BSD.  
> Gestión de usuarios, grupos y políticas en C puro — sin dependencias externas.

```
 _     _ _       ____  _
| |   (_) |_ ___|  _ \(_)_ __
| |   | | __/ _ \ | | | | '__|
| |___| | ||  __/ |_| | | |
|_____|_|\__\___|____/|_|_|
```

---

## ¿Qué es LiteDir?

LiteDir es un directorio de usuarios y grupos diseñado para reemplazar Active Directory en entornos donde los requisitos de Active Directory son excesivos: hardware legacy, sistemas BSD embebidos o redes pequeñas que no justifican un servidor Windows.

**No es un clon de AD.** Es una herramienta de propósito específico que resuelve el problema central: autenticar usuarios, asignarlos a grupos y aplicar políticas — con el menor footprint posible.

---

## Características

- **C99 puro** — sin C++, sin Rust, sin scripting
- **Cero dependencias externas** — solo libc del sistema base
- **Formato binario propio** — archivo `ldstore.db`, sin SQLite, sin LDAP
- **Compilable en i386 32-bit** — hardware de hace 15+ años
- **Footprint mínimo** — binario < 2 MB, memoria en reposo < 8 MB
- **POSIX estricto** — compatible con FreeBSD, OpenBSD, NetBSD
- **Seguro por diseño** — `pledge(2)` / `unveil(2)` en OpenBSD, sin root requerido para operar

---

## Hardware de referencia

El proyecto fue diseñado y validado sobre:

| Componente | Especificación |
|---|---|
| CPU | Intel Celeron D 2.4 GHz (i386, 32-bit) |
| RAM | 2 GB DDR |
| Disco | 300 GB HDD |
| Sistema | OpenBSD 7.0 — arquitectura i386 |
| Compilador | cc (clang 11.0.1) — base system |

Si corre en un Celeron D de 2004, corre en cualquier cosa.

---

## Estado del proyecto

```
Fase 1 — ldstore (motor de datos)          ████████░░  En desarrollo
Fase 2 — Red y protocolo LDP               ░░░░░░░░░░  Planificado
Fase 3 — Motor de políticas                ░░░░░░░░░░  Planificado
Fase 4 — Herramientas de administración    ░░░░░░░░░░  Planificado
Fase 5 — Validación y hardening            ░░░░░░░░░░  Planificado
```

---

## Inicio rápido

### Requisitos

- OpenBSD 7.x, FreeBSD 12+, NetBSD 9+ — o cualquier POSIX con clang/gcc
- `cc` y `make` — presentes en el base system de todos los BSDs
- Sin `pkg_add`, sin `pkg install`, sin `apt`

### Compilar

```sh
git clone https://github.com/tuusuario/litedir.git
cd litedir
make
```

Para compilar explícitamente en 32-bit desde una máquina amd64:

```sh
make CFLAGS="-std=c99 -Wall -Wextra -m32"
```

Verificar el binario:

```sh
file ldctl
# ldctl: ELF 32-bit LSB executable, Intel 80386
```

### Uso básico

```sh
# Usuarios
./ldctl user add  alice secreto123
./ldctl user add  bob   pass456
./ldctl user list
./ldctl user auth alice secreto123   # -> OK
./ldctl user del  bob

# Grupos
./ldctl group add admins
./ldctl group adduser admins alice
./ldctl group members admins
./ldctl group list
```

---

## Arquitectura

```
ldstore.db (archivo binario)
     │
     ├── Header (32 bytes)   — magic "LDST", version, contadores
     ├── Usuarios (× 1024)   — uid, nombre, hash bcrypt, grupos
     └── Grupos   (× 256)    — gid, nombre, lista de miembros
          │
          └── db.c           — pread/pwrite POSIX, sin mmap
               │
               ├── users.c   — CRUD de usuarios
               ├── groups.c  — CRUD de grupos y membresías
               ├── auth.c    — crypt_newhash / crypt_checkpass (OpenBSD)
               └── main.c    — ldctl CLI, parseo de argv[]
```

El archivo `ldstore.db` siempre se crea en el directorio de trabajo. El binario no escribe fuera de donde se ejecuta.

---

## Estructura del repositorio

```
litedir/
├── Makefile
├── include/
│   └── store.h          — estructuras en disco y API pública
├── src/
│   ├── db.c / db.h      — motor de almacenamiento
│   ├── users.c          — gestión de usuarios
│   ├── groups.c         — gestión de grupos
│   ├── auth.c           — hashing y autenticación
│   └── main.c           — ldctl (CLI)
├── tests/
│   └── test_users.c     — pruebas unitarias en C
└── docs/
    └── litedir_fase1.pdf  — guía de inicio detallada
```

---

## Correr las pruebas

```sh
make test
# Todos los tests pasaron.
```

Las pruebas son C puro usando `assert()`. Sin frameworks, sin dependencias.

---

## Seguridad

- Las contraseñas se almacenan con **bcrypt** vía `crypt_newhash(3)` de OpenBSD. Nunca en texto plano ni con MD5/SHA-1.
- El archivo `ldstore.db` se crea con permisos `0600` — solo el propietario puede leerlo.
- En producción el daemon usará `pledge(2)` y `unveil(2)` para restringir sus syscalls al mínimo necesario (Fase 4).
- El binario opera sin privilegios root. No usa `setuid`.

---

## Hoja de ruta

### Fase 1 — ldstore (actual)
Motor de almacenamiento en C puro. CLI `ldctl` para gestión local de usuarios y grupos. Sin red.

### Fase 2 — Red y protocolo LDP
Servidor TCP con event loop basado en `kqueue(2)`. Protocolo binario propio (LiteDir Protocol) con mensajes TLV. TLS opcional via BearSSL embebido.

### Fase 3 — Motor de políticas
Reglas en archivos `.ini`, herencia por grupo, mapeo a permisos POSIX del sistema de archivos.

### Fase 4 — Herramientas de administración
`ldird` (daemon con rc.d), `ldisync` (replicación activo/pasivo entre nodos).

### Fase 5 — Hardening
Fuzzing con AFL++, análisis estático con `scan-build`, pruebas en hardware legacy real.

---

## Diferencias con Active Directory / OpenLDAP

| | Active Directory | OpenLDAP | **LiteDir** |
|---|---|---|---|
| Dependencias | Cientos | Decenas | **Cero** |
| RAM mínima | ~2 GB | ~128 MB | **< 8 MB** |
| Arquitectura | x86_64 | x86_64 / ARM | **i386 / x86_64 / ARM** |
| Protocolo | LDAP + Kerberos | LDAP | **LDP (binario propio)** |
| Lenguaje | C / .NET | C | **C99 puro** |
| BSD nativo | No | Parcial | **Sí** |

---

## Contribuir

El proyecto está en Fase 1. Las contribuciones más útiles ahora mismo son:

1. **Pruebas en hardware real** — especialmente i386 y ARM bajo
2. **Portabilidad** — probar en NetBSD, DragonFlyBSD
3. **Revisión de seguridad** — el formato del archivo y el parser de argumentos
4. **Documentación** — man pages en formato `mandoc`

Antes de abrir un PR, verificar que `make test` pasa y que `cc -Wall -Wextra` no emite warnings.

---

## Licencia

GPL V3

---

<sub>Desarrollado sobre OpenBSD 7.0 · Intel Celeron D 2.4 GHz · cc (clang 11.0.1) · C99</sub>
