# ISO Audit Management Software

A modular C application to efficiently manage and schedule ISO audit records for organizations. Supports validated audit creation, prioritized scheduling, and real-time tracking, leveraging robust data structures and simple interactive controls.

---

## 📂 Project Files

- `iso.h` — Type definitions, constants, and function prototypes  
- `isofunctions.c` — All core audit management and scheduling logic  
- `main.c` — Main user interface and workflow integration

---


## ✨ Key Features

- **Audit Record Management:**  
  Create and store audits with department, company type, category, audit type, and fixed payment options.
- **Validated Scheduling:**  
  Only permitted audit types (Standard, Medium, Fastest) and payment values accepted for scheduling.
- **Priority Queues:**  
  Schedule and display audits based on urgency, ensuring high-priority items are processed first.
- **Undo Capability:**  
  Mistakes in record entry can be quickly reverted via undo stack.
- **Ongoing Audit Tracking:**  
  Track active audits with a dynamic list, updating as audits progress or complete.

---





