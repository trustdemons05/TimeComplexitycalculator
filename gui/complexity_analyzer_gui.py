#!/usr/bin/env python3
"""
C++ Complexity Analyzer GUI
A professional desktop application for analyzing C++ code complexity
"""

import tkinter as tk
from tkinter import ttk, filedialog, scrolledtext, messagebox
import subprocess
import os
import sys
import threading
import json
from pathlib import Path


class ComplexityAnalyzerGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("C++ Complexity Analyzer v6.0")
        self.root.geometry("1200x800")
        self.root.minsize(1000, 700)
        
        # Find the analyzer executable
        self.analyzer_path = self.find_analyzer()
        
        # Size presets for different algorithm speeds
        self.size_presets = {
            "Tiny": "1,5,10,15,20,25",  # For O(2^n) - exponential
            "Small": "10,50,100,500,1000",  # For O(n^2) - quadratic
            "Medium": "100,500,1000,5000,10000",  # For O(n log n) - linearithmic
            "Large": "1000,5000,10000,50000,100000",  # For O(n) - linear (moderate)
            "Huge": "10000,50000,100000,500000,1000000",  # For O(n) - linear (fast)
            "Massive": "100000,500000,1000000,5000000,10000000"  # For O(log n) - logarithmic
        }
        
        # Variables
        self.source_file = tk.StringVar()
        self.function_name = tk.StringVar()
        self.analysis_mode = tk.StringVar(value="both")
        self.timeout = tk.IntVar(value=30)
        self.optimization_level = tk.IntVar(value=0)
        self.input_sizes = tk.StringVar(value="100,500,1000,5000,10000")
        
        # Running state
        self.is_running = False
        
        self.setup_ui()
        
    def find_analyzer(self):
        """Find the complexity analyzer executable"""
        possible_paths = [
            Path("bin/complexity_analyzer.exe"),
            Path("bin/complexity_analyzer"),
            Path("complexity_analyzer.exe"),
            Path("complexity_analyzer")
        ]
        
        for path in possible_paths:
            if path.exists():
                return str(path.absolute())
        
        return None
    
    def setup_ui(self):
        """Setup the user interface"""
        # Configure grid weights for responsive layout
        self.root.grid_rowconfigure(1, weight=1)
        self.root.grid_columnconfigure(0, weight=1)
        
        # Header
        self.create_header()
        
        # Main content area
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=1, column=0, sticky="nsew")
        main_frame.grid_rowconfigure(1, weight=1)
        main_frame.grid_columnconfigure(0, weight=1)
        
        # Input panel
        self.create_input_panel(main_frame)
        
        # Output panel
        self.create_output_panel(main_frame)
        
        # Status bar
        self.create_status_bar()
    
    def create_header(self):
        """Create header with title and analyzer status"""
        header_frame = ttk.Frame(self.root, padding="10", relief="solid", borderwidth=1)
        header_frame.grid(row=0, column=0, sticky="ew")
        header_frame.grid_columnconfigure(1, weight=1)
        
        # Title
        title_label = ttk.Label(
            header_frame,
            text="C++ Complexity Analyzer",
            font=("Segoe UI", 16, "bold")
        )
        title_label.grid(row=0, column=0, sticky="w", padx=5)
        
        # Version
        version_label = ttk.Label(
            header_frame,
            text="v6.0",
            font=("Segoe UI", 10)
        )
        version_label.grid(row=0, column=1, sticky="w", padx=5)
        
        # Analyzer status
        status_text = "✓ Analyzer Ready" if self.analyzer_path else "✗ Analyzer Not Found"
        status_color = "green" if self.analyzer_path else "red"
        
        status_label = ttk.Label(
            header_frame,
            text=status_text,
            foreground=status_color,
            font=("Segoe UI", 10)
        )
        status_label.grid(row=0, column=2, sticky="e", padx=5)
    
    def create_input_panel(self, parent):
        """Create input configuration panel"""
        input_frame = ttk.LabelFrame(parent, text="Configuration", padding="10")
        input_frame.grid(row=0, column=0, sticky="ew", pady=(0, 10))
        input_frame.grid_columnconfigure(1, weight=1)
        
        row = 0
        
        # Source file selection
        ttk.Label(input_frame, text="Source File:").grid(
            row=row, column=0, sticky="w", pady=5, padx=(0, 10)
        )
        ttk.Entry(input_frame, textvariable=self.source_file, width=50).grid(
            row=row, column=1, sticky="ew", pady=5, padx=(0, 5)
        )
        ttk.Button(input_frame, text="Browse...", command=self.browse_file).grid(
            row=row, column=2, pady=5
        )
        row += 1
        
        # Function name
        ttk.Label(input_frame, text="Function Name:").grid(
            row=row, column=0, sticky="w", pady=5, padx=(0, 10)
        )
        ttk.Entry(input_frame, textvariable=self.function_name, width=50).grid(
            row=row, column=1, sticky="ew", pady=5, columnspan=2
        )
        ttk.Label(input_frame, text="(Required for runtime analysis)", font=("Segoe UI", 8)).grid(
            row=row+1, column=1, sticky="w", pady=(0, 5)
        )
        row += 2
        
        # Analysis mode
        ttk.Label(input_frame, text="Analysis Mode:").grid(
            row=row, column=0, sticky="w", pady=5, padx=(0, 10)
        )
        mode_frame = ttk.Frame(input_frame)
        mode_frame.grid(row=row, column=1, sticky="w", pady=5, columnspan=2)
        
        ttk.Radiobutton(mode_frame, text="Static Only", variable=self.analysis_mode, 
                       value="static").pack(side="left", padx=(0, 15))
        ttk.Radiobutton(mode_frame, text="Runtime Only", variable=self.analysis_mode, 
                       value="runtime").pack(side="left", padx=(0, 15))
        ttk.Radiobutton(mode_frame, text="Both (Compare)", variable=self.analysis_mode, 
                       value="both").pack(side="left")
        row += 1
        
        # Advanced options frame
        advanced_frame = ttk.LabelFrame(input_frame, text="Advanced Options", padding="5")
        advanced_frame.grid(row=row, column=0, columnspan=3, sticky="ew", pady=(10, 0))
        advanced_frame.grid_columnconfigure(1, weight=1)
        advanced_frame.grid_columnconfigure(3, weight=1)
        
        # Timeout
        ttk.Label(advanced_frame, text="Timeout (seconds):").grid(
            row=0, column=0, sticky="w", pady=5, padx=(0, 10)
        )
        ttk.Spinbox(advanced_frame, from_=1, to=300, textvariable=self.timeout, width=10).grid(
            row=0, column=1, sticky="w", pady=5, padx=(0, 20)
        )
        
        # Optimization level
        ttk.Label(advanced_frame, text="Optimization Level:").grid(
            row=0, column=2, sticky="w", pady=5, padx=(0, 10)
        )
        opt_combo = ttk.Combobox(advanced_frame, textvariable=self.optimization_level, 
                                 values=[0, 1, 2, 3], width=10, state="readonly")
        opt_combo.grid(row=0, column=3, sticky="w", pady=5)
        
        # Size preset dropdown
        ttk.Label(advanced_frame, text="Size Preset:").grid(
            row=1, column=0, sticky="w", pady=5, padx=(0, 10)
        )
        self.size_preset = tk.StringVar(value="Medium")
        size_preset_combo = ttk.Combobox(
            advanced_frame, 
            textvariable=self.size_preset,
            values=["Tiny", "Small", "Medium", "Large", "Huge", "Massive"],
            width=15,
            state="readonly"
        )
        size_preset_combo.grid(row=1, column=1, sticky="w", pady=5, padx=(0, 20))
        size_preset_combo.bind("<<ComboboxSelected>>", self.on_preset_changed)
        
        # Input sizes
        ttk.Label(advanced_frame, text="Custom Sizes:").grid(
            row=1, column=2, sticky="w", pady=5, padx=(0, 10)
        )
        ttk.Entry(advanced_frame, textvariable=self.input_sizes, width=30).grid(
            row=1, column=3, sticky="ew", pady=5
        )
        ttk.Label(advanced_frame, text="(Or use preset above)", font=("Segoe UI", 8)).grid(
            row=2, column=1, columnspan=3, sticky="w"
        )
        
        # Action buttons
        button_frame = ttk.Frame(input_frame)
        button_frame.grid(row=row+1, column=0, columnspan=3, pady=(15, 0))
        
        self.analyze_btn = ttk.Button(
            button_frame, 
            text="Analyze", 
            command=self.run_analysis,
            width=15
        )
        self.analyze_btn.pack(side="left", padx=5)
        
        self.stop_btn = ttk.Button(
            button_frame, 
            text="Stop", 
            command=self.stop_analysis,
            state="disabled",
            width=15
        )
        self.stop_btn.pack(side="left", padx=5)
        
        ttk.Button(
            button_frame, 
            text="Clear Output", 
            command=self.clear_output,
            width=15
        ).pack(side="left", padx=5)
    
    def create_output_panel(self, parent):
        """Create output display panel"""
        output_frame = ttk.LabelFrame(parent, text="Analysis Results", padding="10")
        output_frame.grid(row=1, column=0, sticky="nsew")
        output_frame.grid_rowconfigure(0, weight=1)
        output_frame.grid_columnconfigure(0, weight=1)
        
        # Create notebook for tabbed output
        self.notebook = ttk.Notebook(output_frame)
        self.notebook.grid(row=0, column=0, sticky="nsew")
        
        # Console output tab
        console_frame = ttk.Frame(self.notebook)
        self.notebook.add(console_frame, text="Console Output")
        console_frame.grid_rowconfigure(0, weight=1)
        console_frame.grid_columnconfigure(0, weight=1)
        
        self.console_output = scrolledtext.ScrolledText(
            console_frame,
            wrap=tk.WORD,
            font=("Consolas", 10),
            bg="#1e1e1e",
            fg="#d4d4d4",
            insertbackground="white"
        )
        self.console_output.grid(row=0, column=0, sticky="nsew")
        
        # Results summary tab
        summary_frame = ttk.Frame(self.notebook)
        self.notebook.add(summary_frame, text="Summary")
        summary_frame.grid_rowconfigure(0, weight=1)
        summary_frame.grid_columnconfigure(0, weight=1)
        
        self.summary_output = scrolledtext.ScrolledText(
            summary_frame,
            wrap=tk.WORD,
            font=("Segoe UI", 10),
            bg="white"
        )
        self.summary_output.grid(row=0, column=0, sticky="nsew")
    
    def create_status_bar(self):
        """Create status bar at bottom"""
        self.status_bar = ttk.Label(
            self.root,
            text="Ready",
            relief="sunken",
            anchor="w",
            padding="5"
        )
        self.status_bar.grid(row=2, column=0, sticky="ew")
    
    def on_preset_changed(self, event=None):
        """Handle size preset selection"""
        preset = self.size_preset.get()
        if preset in self.size_presets:
            self.input_sizes.set(self.size_presets[preset])
            self.update_status(f"Size preset: {preset}")
    
    def browse_file(self):
        """Open file browser to select source file"""
        filename = filedialog.askopenfilename(
            title="Select C++ Source File",
            filetypes=[
                ("C++ Files", "*.cpp *.cc *.cxx"),
                ("C Files", "*.c"),
                ("Header Files", "*.h *.hpp"),
                ("All Files", "*.*")
            ]
        )
        if filename:
            self.source_file.set(filename)
            self.update_status(f"Selected: {os.path.basename(filename)}")
    
    def validate_inputs(self):
        """Validate user inputs before running analysis"""
        if not self.source_file.get():
            messagebox.showerror("Error", "Please select a source file")
            return False
        
        if not os.path.exists(self.source_file.get()):
            messagebox.showerror("Error", "Source file does not exist")
            return False
        
        if self.analysis_mode.get() in ["runtime", "both"]:
            if not self.function_name.get():
                messagebox.showerror("Error", "Function name is required for runtime analysis")
                return False
        
        if not self.analyzer_path:
            messagebox.showerror(
                "Error", 
                "Complexity analyzer executable not found.\n\n"
                "Please build the project first:\n"
                "  Windows: build.bat\n"
                "  Linux/Mac: ./build.sh"
            )
            return False
        
        return True
    
    def run_analysis(self):
        """Run the complexity analysis"""
        if not self.validate_inputs():
            return
        
        # Disable analyze button, enable stop button
        self.analyze_btn.config(state="disabled")
        self.stop_btn.config(state="normal")
        self.is_running = True
        
        # Clear previous output
        self.console_output.delete(1.0, tk.END)
        self.summary_output.delete(1.0, tk.END)
        
        # Build command
        cmd = [self.analyzer_path]
        
        # Add mode flag
        if self.analysis_mode.get() == "static":
            cmd.append("--static")
        elif self.analysis_mode.get() == "runtime":
            cmd.append("--runtime")
        else:
            cmd.append("--both")
        
        # Add function name if provided
        if self.function_name.get():
            cmd.extend(["--function", self.function_name.get()])
        
        # Add timeout
        cmd.extend(["--timeout", str(self.timeout.get())])
        
        # Add optimization level
        cmd.extend(["--optimization", str(self.optimization_level.get())])
        
        # Add input sizes
        if self.input_sizes.get():
            cmd.extend(["--sizes", self.input_sizes.get()])
        
        # Add source file
        cmd.append(self.source_file.get())
        
        # Update status
        self.update_status("Running analysis...")
        self.log_console(f"Command: {' '.join(cmd)}\n\n")
        
        # Run in separate thread
        thread = threading.Thread(target=self.execute_analysis, args=(cmd,))
        thread.daemon = True
        thread.start()
    
    def execute_analysis(self, cmd):
        """Execute the analysis command in a separate thread"""
        try:
            process = subprocess.Popen(
                cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                bufsize=1,
                universal_newlines=True
            )
            
            # Read output line by line
            for line in process.stdout:
                if not self.is_running:
                    process.terminate()
                    break
                self.log_console(line)
            
            process.wait()
            
            if self.is_running:
                if process.returncode == 0:
                    self.update_status("Analysis completed successfully")
                    self.parse_results()
                else:
                    self.update_status(f"Analysis failed with code {process.returncode}")
            else:
                self.update_status("Analysis stopped by user")
        
        except Exception as e:
            self.log_console(f"\nError: {str(e)}\n")
            self.update_status(f"Error: {str(e)}")
        
        finally:
            # Re-enable buttons
            self.root.after(0, self.reset_buttons)
    
    def stop_analysis(self):
        """Stop the running analysis"""
        self.is_running = False
        self.update_status("Stopping analysis...")
    
    def reset_buttons(self):
        """Reset button states after analysis"""
        self.analyze_btn.config(state="normal")
        self.stop_btn.config(state="disabled")
        self.is_running = False
    
    def clear_output(self):
        """Clear all output"""
        self.console_output.delete(1.0, tk.END)
        self.summary_output.delete(1.0, tk.END)
        self.update_status("Output cleared")
    
    def log_console(self, text):
        """Log text to console output (thread-safe)"""
        self.root.after(0, self._log_console_impl, text)
    
    def _log_console_impl(self, text):
        """Implementation of console logging"""
        self.console_output.insert(tk.END, text)
        self.console_output.see(tk.END)
    
    def update_status(self, text):
        """Update status bar (thread-safe)"""
        self.root.after(0, self._update_status_impl, text)
    
    def _update_status_impl(self, text):
        """Implementation of status update"""
        self.status_bar.config(text=text)
    
    def parse_results(self):
        """Parse console output and create summary"""
        output = self.console_output.get(1.0, tk.END)
        
        summary = []
        summary.append("=" * 60)
        summary.append("ANALYSIS SUMMARY")
        summary.append("=" * 60)
        summary.append("")
        
        # Extract key information
        lines = output.split('\n')
        
        # Look for complexity results
        static_complexity = None
        runtime_complexity = None
        match_status = None
        accuracy = None
        
        for i, line in enumerate(lines):
            if "Static Analysis:" in line or "Complexity:" in line:
                if i + 1 < len(lines):
                    static_complexity = line.strip()
            elif "Runtime Profiling:" in line or "Detected:" in line:
                if i + 1 < len(lines):
                    runtime_complexity = line.strip()
            elif "Match:" in line:
                match_status = line.strip()
            elif "Accuracy:" in line:
                accuracy = line.strip()
        
        # Add to summary
        if static_complexity:
            summary.append(f"📊 {static_complexity}")
        if runtime_complexity:
            summary.append(f"⏱️  {runtime_complexity}")
        if match_status:
            summary.append(f"✓  {match_status}")
        if accuracy:
            summary.append(f"🎯 {accuracy}")
        
        summary.append("")
        summary.append("=" * 60)
        summary.append("")
        summary.append("See Console Output tab for detailed results.")
        
        # Display summary
        self.summary_output.delete(1.0, tk.END)
        self.summary_output.insert(1.0, '\n'.join(summary))


def main():
    """Main entry point"""
    root = tk.Tk()
    
    # Set theme
    style = ttk.Style()
    style.theme_use('clam')
    
    app = ComplexityAnalyzerGUI(root)
    
    # Center window on screen
    root.update_idletasks()
    width = root.winfo_width()
    height = root.winfo_height()
    x = (root.winfo_screenwidth() // 2) - (width // 2)
    y = (root.winfo_screenheight() // 2) - (height // 2)
    root.geometry(f'{width}x{height}+{x}+{y}')
    
    root.mainloop()


if __name__ == "__main__":
    main()
