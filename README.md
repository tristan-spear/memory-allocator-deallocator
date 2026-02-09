This is my own implementation of Malloc & Free - working memory allocation - in C
<br/>
This was an assignment for CSC 357 (Systems Programming) at Cal Poly
<hr/>
What's included :
<ul>
  <li>Assignment prompt (pdf from Prof. Christian Echkardt)</li>
  <li>C files (with actual program break moving)</li>
  <li>C++ files (with simulated program break movement)</li>
</ul>
<hr/>
Implementation Details :
<br/><br/>
<ul>
  <li>Implemented a custom heap using a doubly linked list of memory chunks, with in-band metadata storing block size, allocation state, and neighboring pointers</li>
  <br/>
  <li>Used a best-fit allocation strategy to select the smallest free block that satisfies a request, reducing fragmentation</li>
  <br/>
  <li>Supported block splitting when allocating from oversized free chunks and coalescing adjacent free blocks during free() operations</li>
  <br/>
  <li>Managed heap growth and shrinkage by moving the program break forward when needed and returning memory when the final chunk is freed</li>
  <br/>
  <li>Provided an analyze() utility to inspect heap structure, block layout, and current program break for debugging and validation</li>
  <br/>
  <li>Created a C++ version with a simulated heap and program break to allow testing on macOS, while preserving low-level memory management behavior</li>
</ul>
<hr/>
