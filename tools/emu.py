import sys
import re
import tkinter as tk
from tkinter import messagebox

class BitwiseEmulator:
    def __init__(self, asm_file):
        self.asm_file = asm_file
        self.regs = {f"%r{i}": 0 for i in range(32)}
        self.regs["R0"] = 0 # Return register alias
        self.mem = {} # Address -> Value
        self.labels = {}
        self.instructions = []
        self.pc = 0
        self.vram_base = 0x4000
        self.vram_ptr = 0
        self.width = 128
        self.height = 64
        
        # Virtual Hardware Registers (like a DMA or PDI)
        self.ADDR_REG = 0x3C00
        self.DATA_REG = 0x3C04
        self.INPUT_REG = 0x3C08
        self.last_key = 0
        
        # GUI
        self.root = tk.Tk()
        self.root.bind("<KeyPress>", self.on_key)
        self.root.title("Bitwise Hardware Emulator")
        self.canvas = tk.Canvas(self.root, width=self.width*4, height=self.height*4, bg="black")
        self.canvas.pack()
        
        self.load_asm()

    def load_asm(self):
        try:
            with open(self.asm_file, 'r') as f:
                lines = f.readlines()
            
            idx = 0
            for line in lines:
                line = line.strip()
                if not line or line.startswith(";"): continue
                
                # Label check
                if line.endswith(":"):
                    label = line[:-1]
                    self.labels[label] = idx
                    continue
                
                # Instruction
                self.instructions.append(line)
                idx += 1
        except Exception as e:
            print(f"Error loading ASM: {e}")

    def step(self):
        if self.pc >= len(self.instructions):
            return False
            
        inst = self.instructions[self.pc]
        self.pc += 1
        
        # Parse instruction
        match = re.match(r"(\w+)\s+(.*)", inst)
        if not match: return True # Just skip unknown
        
        op = match.group(1)
        args = [a.strip() for a in match.group(2).split(",")]
        
        if op == "MOV":
            # MOV %reg, #val OR MOV %reg, %reg
            dest = args[0]
            val = args[1]
            if val.startswith("#"):
                self.regs[dest] = int(val[1:])
            else:
                self.regs[dest] = self.get_val(val)
        
        elif op == "STR":
            # STR %src, [addr] OR STR %src, [base, #offset]
            val = self.get_val(args[0])
            
            addr_str = args[1].strip("[]")
            addr = self.get_addr(addr_str)
            
            self.mem[addr] = val
            
            # Virtual Hardware Trigger
            if addr == self.ADDR_REG:
                self.vram_ptr = val
            elif addr == self.DATA_REG:
                if self.vram_ptr >= self.vram_base:
                    self.update_pixel(self.vram_ptr, val)
                self.vram_ptr += 1 # Auto-increment like a real display
            
            if addr >= self.vram_base:
                self.update_pixel(addr, val)
                
        elif op == "LDR":
            # LDR %dest, [addr]
            dest = args[0]
            addr_str = args[1].strip("[]")
            addr = self.get_addr(addr_str)
            
            # MMIO Read Intercept
            val = 0
            if addr == self.INPUT_REG:
                val = self.last_key
            else:
                val = self.mem.get(addr, 0)
            
            self.regs[dest] = val

        elif op == "ADD":
            # ADD %dest, %src1, %src2
            self.regs[args[0]] = self.get_val(args[1]) + self.get_val(args[2])

        elif op == "SUB":
            # SUB %dest, %src1, %src2
            self.regs[args[0]] = self.get_val(args[1]) - self.get_val(args[2])

        elif op == "SLT":
            self.regs[args[0]] = 1 if self.get_val(args[1]) < self.get_val(args[2]) else 0
        elif op == "SGT":
            self.regs[args[0]] = 1 if self.get_val(args[1]) > self.get_val(args[2]) else 0
        elif op == "SEQ":
            self.regs[args[0]] = 1 if self.get_val(args[1]) == self.get_val(args[2]) else 0
        elif op == "SNE":
            self.regs[args[0]] = 1 if self.get_val(args[1]) != self.get_val(args[2]) else 0

        elif op == "CMP":
            # CMP %reg, #val
            self.last_cmp = self.get_val(args[0]) - self.get_val(args[1])

        elif op == "BNE":
            # BNE label
            if hasattr(self, 'last_cmp') and self.last_cmp != 0:
                self.pc = self.labels.get(args[0], self.pc)

        elif op == "B":
            # B label
            self.pc = self.labels.get(args[0], self.pc)

        elif op == "BX":
             if args[0] == "LR":
                 return False # End of program
        
        return True

    def get_val(self, s):
        if s.startswith("#"): return int(s[1:])
        return self.regs.get(s, 0)

    def get_addr(self, s):
        # Handle [%reg, #member] or [name]
        if "," in s:
            parts = [p.strip() for p in s.split(",")]
            base = self.get_val(parts[0])
            # Struct member offsets
            offsets = {"#addr": 0, "#v_addr": 0, "#data": 4, "#v_data": 4, "#input": 8}
            offset = offsets.get(parts[1], 0)
            return base + offset
        
        # If it's a register
        if s.startswith("%"): return self.regs.get(s, 0)
        
        # Hardware Reads
        addr = self.mem.get(s, 0)
        if addr == self.INPUT_REG:
            return self.last_key

        # If it's a named variable, we give it a dummy address in RAM if not exists
        if s not in self.mem:
            self.mem[s] = 0x1000 + len(self.mem) * 4
        return self.mem[s]

    def update_pixel(self, addr, val):
        # VRAM mapping: 0x4000 + (y * 128 + x)
        offset = addr - self.vram_base
        if offset < 0: return
        x = offset % self.width
        y = offset // self.width
        # Scale by 4 for visibility
        color = "cyan" if val != 0 else "black"
        self.canvas.create_rectangle(x*4, y*4, (x+1)*4, (y+1)*4, fill=color, outline="")

    def on_key(self, event):
        # 1: Up, 2: Down, 3: Left, 4: Right
        if event.keysym == "Up": self.last_key = 1
        elif event.keysym == "Down": self.last_key = 2
        elif event.keysym == "Left": self.last_key = 3
        elif event.keysym == "Right": self.last_key = 4

    def run(self):
        def loop():
            # Run a batch of instructions per frame to simulate speed
            # 500 instructions per 10ms = ~50 KHz (slow but visible)
            for _ in range(500):
                if not self.step():
                    messagebox.showinfo("Bitwise Emulator", "Program Executed Successfully")
                    return
            self.root.after(10, loop)
        
        self.root.after(100, loop)
        self.root.mainloop()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python emu.py <asm_file>")
    else:
        emu = BitwiseEmulator(sys.argv[1])
        emu.run()
