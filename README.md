# Treap Visual – Implementación y Visualización en C++ y Qt

Este proyecto implementa un *Treap* (árbol binario de búsqueda con prioridades tipo heap) en C++ y proporciona una interfaz gráfica en *Qt* para visualizar y manipular la estructura en tiempo real. Permite trabajar con múltiples Treaps, realizar todas las operaciones fundamentales y observar los cambios directamente en la representación gráfica.


## Características principales

- Implementación completa del Treap en C++.
- Visualización estructural mediante QGraphicsScene.
- Manejo simultáneo de varios Treaps.
- Operaciones soportadas:
  - Búsqueda
  - Inserción
  - Eliminación
  - Split (división del Treap en dos)
  - Merge / Join (unión de dos Treaps)

Cada operación actualiza la visualización automáticamente.

---

## Estructura del proyecto



Treap_visual/
│
├── main.cpp
├── treap.h
├── mainwindow.cpp
├── mainwindow.h
├── mainwindow.ui
├── ZoomGraphicsView.h
│
├── Treap_visual.pro
└── build/      (generado automáticamente por Qt Creator)



---

## Compilación

### Requisitos
- Qt 5 o Qt 6
- Qt Creator
- QMake (archivo .pro)

### Pasos
1. Abrir Qt Creator.
2. File → Open File or Project.
3. Seleccionar Treap_visual.pro.
4. Elegir el kit de compilación.
5. Build → Run.

Qt generará la carpeta build/ con los binarios y archivos intermedios.

---

## Uso de la aplicación

### Panel izquierdo
Lista de Treaps disponibles.  
Permite crear, seleccionar y eliminar Treaps.

### Panel central
Visualización del Treap activo:
- La clave aparece en rojo.
- Las prioridades se muestran en azul.
- Las conexiones entre nodos se representan con aristas.
- La estructura se actualiza luego de cada operación.

### Panel inferior – Operaciones del Treap actual
- Insertar nodo (clave + prioridad)
- Buscar nodo
- Eliminar nodo
- Split: divide el Treap actual en dos Treaps nuevos

### Panel inferior – Operaciones globales
- Crear un nuevo Treap vacío
- Eliminar el Treap actual
- Join: unir dos Treaps seleccionados en uno nuevo

---

## Complejidad esperada

| Operación       | Caso promedio | Peor caso |
|-----------------|---------------|-----------|
| Búsqueda        | O(log n)      | O(n)      |
| Inserción       | O(log n)      | O(n)      |
| Eliminación     | O(log n)      | O(n)      |
| Split           | O(log n)      | O(n)      |
| Join            | O(log n)      | O(n)      |

El Treap se mantiene balanceado en promedio gracias a las prioridades aleatorias asignadas a cada nodo.

---

## Qué es un Treap

Un Treap combina dos propiedades:

- *BST (Binary Search Tree):*  
  El orden por clave decide si un valor va a la izquierda o derecha.

- *Heap por prioridad:*  
  La prioridad decide la posición final, garantizando balanceo probabilístico mediante rotaciones.

Esto permite mantener una estructura eficiente sin reglas complejas como en AVL o Red-Black Trees.

---

## Licencia

Proyecto con fines académicos y educativos.
