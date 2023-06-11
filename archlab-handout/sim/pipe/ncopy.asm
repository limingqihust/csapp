#/* $begin ncopy-ys */
##################################################################
# ncopy.ys - Copy a src block of len words to dst.
# Return the number of positive words (>0) contained in src.
#
# Include your name and ID here.
#
# Describe how and why you modified the baseline code.
#
##################################################################

# Do not modify this portion
# Function prologue.
														# %rdi = src 
														# %rsi = dst 
														# %rdx = len

                                                        # %rax = count
                                                        # %r8 = val
                                                        
ncopy:

##################################################################
# You can modify this portion
	# Loop header
	xorq %rax,%rax										# count = 0;

######## Loop1 begin
    jmp TEST1
LOOP1:
    
# 1
COPY1:
	mrmovq (%rdi), %r8
    mrmovq 8(%rdi), %r9							
	rmmovq %r8, (%rsi)	
    rmmovq %r9,8(%rsi)								  
	andq %r8, %r8									
	jle COPY2											
    iaddq $1,%rax                                       
# 2                       
COPY2: 	
    andq %r9, %r9										# val <= 0?												
	jle COPY3											
    iaddq $1,%rax                                       # count++    
# 3         
COPY3:
    mrmovq 16(%rdi), %r8
    mrmovq 24(%rdi), %r9							
	rmmovq %r8,16(%rsi)	
    rmmovq %r9,24(%rsi)								  
	andq %r8, %r8									
	jle COPY4											
    iaddq $1,%rax     
# 4
COPY4:
    andq %r9, %r9										# val <= 0?
	jle COPY5											# 
    iaddq $1,%rax                                       # count++   

# 5
COPY5:
    mrmovq 32(%rdi), %r8								
    mrmovq 40(%rdi), %r9
	rmmovq %r8, 32(%rsi)								
    rmmovq %r9, 40(%rsi) 
    andq %r8, %r8		
	jle COPY6											
    iaddq $1,%rax                                  

# 6
COPY6:							 
    iaddq $48,%rsi                                   
    iaddq $48,%rdi
    andq %r9, %r9										
	jle TEST1											 
    iaddq $1,%rax                                        
TEST1:
    iaddq $-6,%rdx
    jge LOOP1
    iaddq $6,%rdx     
######## Loop1 end

######## Loop2 begin
    jmp TEST2
LOOP2:
    
COPY7:
	mrmovq (%rdi), %r8
    mrmovq 8(%rdi), %r9
    mrmovq 16(%rdi), %r10	
    rmmovq %r8, (%rsi)	
    rmmovq %r9, 8(%rsi)	
    rmmovq %r10, 16(%rsi)	
    
    andq %r8, %r8																		  			
	jle COPY8											
    iaddq $1,%rax  
COPY8:
    andq %r9, %r9																		  			
	jle COPY9											
    iaddq $1,%rax  
COPY9:
    iaddq $24,%rsi    
    iaddq $24,%rdi 									  					
	andq %r10, %r10
    jle TEST2											
    iaddq $1,%rax  

TEST2:
    iaddq $-3,%rdx
    jge LOOP2
    iaddq $3,%rdx



######## Loop2 end

######## Loop3 begin
    jmp TEST3
LOOP3:     
    mrmovq (%rdi), %r8  
    rmmovq %r8, (%rsi) 
    iaddq $8,%rdi
    iaddq $8,%rsi
    andq %r8, %r8
    jle TEST3        
    iaddq $1,%rax     
TEST3:
    iaddq $-1,%rdx
    jge LOOP3

######## Loop3 end

##################################################################
# Do not modify the following section of code
# Function epilogue.

	ret
##################################################################
# Keep the following label at the end of your function
End:
#/* $end ncopy-ys */
