import pandas as pd
import matplotlib.pyplot as plt

# (b) k=18
fig, ejes = plt.subplots(1, 2, figsize=(12, 5))
for eje, nombre, titulo in zip(ejes, ["brute", "kd"], ["Fuerza bruta", "KD-Tree"]):
    pts = pd.read_csv(f"clusters_{nombre}.csv")
    cen = pd.read_csv(f"centroides_{nombre}.csv")
    eje.scatter(pts["x"], pts["y"], c=pts["cluster"], cmap="tab20", s=8)
    eje.scatter(cen["x"], cen["y"], c="black", marker="X", s=120, edgecolors="white")
    eje.set_title(f"k-means {titulo} (k=18)")
plt.tight_layout()
plt.savefig("fig_b_clusters.png", dpi=150)

# tiempos de las 10 ejecuciones
r = pd.read_csv("resultados_b.csv")
plt.figure(figsize=(7, 4))
plt.plot(r["ejecucion"], r["brute_ms"], marker="o", label="fuerza bruta")
plt.plot(r["ejecucion"], r["kd_ms"], marker="o", label="KD-Tree")
plt.xlabel("ejecucion"); plt.ylabel("tiempo (ms)"); plt.title("10 ejecuciones, k=18")
plt.legend(); plt.tight_layout()
plt.savefig("fig_b_tiempos.png", dpi=150)

# (c) k fijo, n variable
df = pd.read_csv("tiempos_vs_n.csv")
ks = sorted(df["k"].unique())
fig, ejes = plt.subplots(1, len(ks), figsize=(4 * len(ks), 3.5))
for eje, k in zip(ejes, ks):
    s = df[df["k"] == k]
    eje.plot(s["n"], s["brute_ms"], label="brute-force")
    eje.plot(s["n"], s["kd_ms"], label="k-d tree")
    eje.set_title(f"k={k}"); eje.set_xlabel("n"); eje.set_ylabel("tiempo (ms)")
ejes[0].legend()
plt.tight_layout()
plt.savefig("fig_c_tiempo_vs_n.png", dpi=150)

# (d) n fijo, k variable
df = pd.read_csv("tiempos_vs_k.csv")
ns = sorted(df["n"].unique())
fig, ejes = plt.subplots(1, len(ns), figsize=(4 * len(ns), 3.5))
for eje, n in zip(ejes, ns):
    s = df[df["n"] == n]
    eje.plot(s["k"], s["brute_ms"], label="brute-force")
    eje.plot(s["k"], s["kd_ms"], label="k-d tree")
    eje.set_title(f"n={n}"); eje.set_xlabel("k"); eje.set_ylabel("tiempo (ms)")
ejes[0].legend()
plt.tight_layout()
plt.savefig("fig_d_tiempo_vs_k.png", dpi=150)
print("Graficos guardados")