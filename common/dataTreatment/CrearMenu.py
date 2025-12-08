import pandas as pd
import random

df = pd.read_csv("DataRestaurante.csv")

platos_df = df[["item_name", "item_price"]].drop_duplicates(subset="item_name")

platos_df["tiempo_preparacion_min"] = [random.randint(10, 30) for _ in range(len(platos_df))]

def asignar_categoria(nombre):
    nombre = nombre.lower()
    if "juice" in nombre:
        return "Bebida"
    elif "frankie" in nombre or "sandwich" in nombre:
        return "Principal"
    elif "vadapav" in nombre or "panipuri" in nombre or "aalopuri" in nombre:
        return "Snack"
    else:
        return "Otro"

platos_df["categoria"] = platos_df["item_name"].apply(asignar_categoria)

platos_df.insert(0, "id_plato", range(1, len(platos_df) + 1))

platos_df.to_csv("menu_generado.csv", index=False)

print("Archivo 'menu_generado.csv' creado sin duplicados.")
