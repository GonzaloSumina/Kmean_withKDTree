# K-Means con KD-Tree

Implementación y comparativa de rendimiento entre el algoritmo **K-Means** tradicional (fuerza bruta) y **K-Means optimizado con un KD-Tree** sobre los centroides en **C++17**, acompañado de un script en **Python** para visualizar los resultados y tiempos de ejecución.

## Estructura del proyecto

- `main.cpp`: Código fuente en C++17. Lee `data2k.csv`, ejecuta los experimentos comparativos (variando ejecuciones con $k=18$, variando $n$ con $k$ fijo, y variando $k$ con $n$ fijo) y exporta los resultados en archivos `.csv`.
- `Graficar.py`: Script en Python (`pandas` y `matplotlib`) que lee los `.csv` generados por el ejecutable y produce las gráficas comparativas (`fig_b_clusters.png`, `fig_b_tiempos.png`, `fig_c_tiempo_vs_n.png`, `fig_d_tiempo_vs_k.png`).
- `data2k.csv`: Dataset de entrada utilizado para las pruebas.
- `KMean_withKDTree.cbp`: Archivo de proyecto para Code::Blocks (opcional).

---

## Requisitos

- Compilador de C++ con soporte para **C++17** (`g++`).
- **Python 3** con `pip` y módulo `venv`.

---

## Ejecución paso a paso

### 1. Clonar el repositorio y entrar a la carpeta

```bash
git clone <URL_DEL_REPOSITORIO>
cd KMean_withKDTree
```

### 2. Crear y activar el entorno virtual de Python

**En Linux / macOS:**
```bash
python3 -m venv .venv
source .venv/bin/activate
pip install pandas matplotlib
```

**En Windows (PowerShell):**
```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install pandas matplotlib
```

### 3. Compilar y ejecutar el programa en C++

```bash
g++ -std=c++17 -O2 main.cpp -o main
./main
```
*(En Windows, ejecuta `.\main.exe` en lugar de `./main`).*

Esto generará automáticamente los archivos `clusters_*.csv`, `centroides_*.csv`, `resultados_b.csv`, `tiempos_vs_n.csv` y `tiempos_vs_k.csv`.

### 4. Generar las gráficas

Con el entorno virtual `.venv` activado:

```bash
python Graficar.py
```

Al finalizar, se guardarán en la misma carpeta las imágenes `fig_*.png` con los clusters y las curvas de rendimiento.