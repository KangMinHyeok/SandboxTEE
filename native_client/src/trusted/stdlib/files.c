#ifndef FILES
#define FILES


#include "files.h"

FILE *
fopen(const char *file, const char *mode)
{
  FILE *fp;
  int f;
  int flags, oflags;
  if ((flags = __sflags(mode, &oflags)) == 0)
    return (NULL);
  if ((fp = __sfp()) == NULL)
    return (NULL);
  if ((f = open(file, oflags, DEFFILEMODE)) < 0) {
    fp->_flags = 0;     /* release */
    return (NULL);
  }
  fp->_file = f;
  fp->_flags = flags;
  fp->_cookie = fp;
  fp->_read = __sread;
  fp->_write = __swrite;
  fp->_seek = __sseek;
  fp->_close = __sclose;
  /*
   * When opening in append mode, even though we use O_APPEND,
   * we need to seek to the end so that ftell() gets the right
   * answer.  If the user then alters the seek pointer, or
   * the file extends, this will fail, but there is not much
   * we can do about this.  (We could set __SAPP and check in
   * fseek and ftell.)
   */
  if (oflags & O_APPEND)
    (void) __sseek((void *)fp, (fpos_t)0, SEEK_END);
  return (fp);
}


#define POS_ERR (-(fpos_t)1)
/*
 *  * Seek the given file to the given offset.
 *   * `Whence' must be one of the three SEEK_* macros.
 *    */
  int
fseeko(FILE *fp, off_t offset, int whence)
{
  fpos_t (*seekfn)(void *, fpos_t, int);
  fpos_t target, curoff;
  size_t n;
  struct stat st;
  int havepos;
  /* make sure stdio is set up */
  if (!__sdidinit)
    __sinit();
  /*
   *   * Have to be able to seek.
   *     */
  if ((seekfn = fp->_seek) == NULL) {
    errno = ESPIPE;     /* historic practice */
    return (EOF);
  }
  /*
   *   * Change any SEEK_CUR to SEEK_SET, and check `whence' argument.
   *     * After this, whence is either SEEK_SET or SEEK_END.
   *       */
  switch (whence) {
    case SEEK_CUR:
      /*
       *     * In order to seek relative to the current stream offset,
       *         * we have to first find the current stream offset a la
       *             * ftell (see ftell for details).
       *                 */
      __sflush(fp); /* may adjust seek offset on append stream */
      if (fp->_flags & __SOFF)
        curoff = fp->_offset;
      else {
        curoff = (*seekfn)(fp->_cookie, (fpos_t)0, SEEK_CUR);
        if (curoff == (fpos_t)-1)
          return (EOF);
      }
      if (fp->_flags & __SRD) {
        curoff -= fp->_r;
        if (HASUB(fp))
          curoff -= fp->_ur;
      } else if (fp->_flags & __SWR && fp->_p != NULL)
        curoff += fp->_p - fp->_bf._base;
      offset += curoff;
      whence = SEEK_SET;
      havepos = 1;
      break;
    case SEEK_SET:
    case SEEK_END:
      curoff = 0;   /* XXX just to keep gcc quiet */
      havepos = 0;
      break;
    default:
      errno = EINVAL;
      return (EOF);
  }
  /*
   *   * Can only optimise if:
   *     *  reading (and not reading-and-writing);
   *       *  not unbuffered; and
   *         *  this is a `regular' Unix file (and hence seekfn==__sseek).
   *           * We must check __NBF first, because it is possible to have __NBF
   *             * and __SOPT both set.
   *               */
  if (fp->_bf._base == NULL)
    __smakebuf(fp);
  if (fp->_flags & (__SWR | __SRW | __SNBF | __SNPT))
    goto dumb;
  if ((fp->_flags & __SOPT) == 0) {
    if (seekfn != __sseek ||
        fp->_file < 0 || fstat(fp->_file, &st) ||
        (st.st_mode & S_IFMT) != S_IFREG) {
      fp->_flags |= __SNPT;
      goto dumb;
    }
    fp->_blksize = st.st_blksize;
    fp->_flags |= __SOPT;
  }
  /*
   *   * We are reading; we can try to optimise.
   *     * Figure out where we are going and where we are now.
   *       */
  if (whence == SEEK_SET)
    target = offset;
  else {
    if (fstat(fp->_file, &st))
      goto dumb;
    target = st.st_size + offset;
  }
  if (!havepos) {
    if (fp->_flags & __SOFF)
      curoff = fp->_offset;
    else {
      curoff = (*seekfn)(fp->_cookie, (fpos_t)0, SEEK_CUR);
      if (curoff == POS_ERR)
        goto dumb;
    }
    curoff -= fp->_r;
    if (HASUB(fp))
      curoff -= fp->_ur;
  }
  /*
   *   * Compute the number of bytes in the input buffer (pretending
   *     * that any ungetc() input has been discarded).  Adjust current
   *       * offset backwards by this count so that it represents the
   *         * file offset for the first byte in the current input buffer.
   *           */
  if (HASUB(fp)) {
    curoff += fp->_r; /* kill off ungetc */
    n = fp->_up - fp->_bf._base;
    curoff -= n;
    n += fp->_ur;
  } else {
    n = fp->_p - fp->_bf._base;
    curoff -= n;
    n += fp->_r;
  }
  /*
   *   * If the target offset is within the current buffer,
   *     * simply adjust the pointers, clear EOF, undo ungetc(),
   *       * and return.  (If the buffer was modified, we have to
   *         * skip this; see fgetln.c.)
   *           */
  if ((fp->_flags & __SMOD) == 0 &&
      target >= curoff && target < (fpos_t)(curoff + n)) {
    int o = target - curoff;
    fp->_p = fp->_bf._base + o;
    fp->_r = n - o;
    if (HASUB(fp))
      FREEUB(fp);
    fp->_flags &= ~__SEOF;
    return (0);
  }
  /*
   *   * The place we want to get to is not within the current buffer,
   *     * but we can still be kind to the kernel copyout mechanism.
   *       * By aligning the file offset to a block boundary, we can let
   *         * the kernel use the VM hardware to map pages instead of
   *           * copying bytes laboriously.  Using a block boundary also
   *             * ensures that we only read one block, rather than two.
   *               */
  curoff = target & ~(fp->_blksize - 1);
  if ((*seekfn)(fp->_cookie, curoff, SEEK_SET) == POS_ERR)
    goto dumb;
  fp->_r = 0;
  fp->_p = fp->_bf._base;
  if (HASUB(fp))
    FREEUB(fp);
  fp->_flags &= ~__SEOF;
  n = target - curoff;
  if (n) {
    if (__srefill(fp) || (size_t)fp->_r < n)
      goto dumb;
    fp->_p += n;
    fp->_r -= n;
  }
  return (0);
  /*
   *   * We get here if we cannot optimise the seek ... just
   *     * do it.  Allow the seek function to change fp->_bf._base.
   *       */
dumb:
  if (__sflush(fp) ||
      (*seekfn)(fp->_cookie, (fpos_t)offset, whence) == POS_ERR) {
    return (EOF);
  }
  /* success: clear EOF indicator and discard ungetc() data */
  if (HASUB(fp))
    FREEUB(fp);
  fp->_p = fp->_bf._base;
  fp->_r = 0;
  /* fp->_w = 0; */ /* unnecessary (I think...) */
  fp->_flags &= ~__SEOF;
  return (0);
}
/*
 *  * fseek()'s offset is a long and sizeof(off_t) != sizeof(long) on all arches
 *   */
#if defined(__alpha__) && defined(__indr_reference)
__indr_reference(fseeko, fseek);
#else
  int
fseek(FILE *fp, long offset, int whence)
{
  off_t off = offset;
  return(fseeko(fp, off, whence));
}

static int
lflush(FILE *fp)
{
    if ((fp->_flags & (__SLBF|__SWR)) == (__SLBF|__SWR))
        return (__sflush(fp));
    return (0);
}
size_t
fread(void *buf, size_t size, size_t count, FILE *fp)
{
    size_t resid;
    char *p;
    int r;
    size_t total;
    /*
     * The ANSI standard requires a return value of 0 for a count
     * or a size of 0.  Peculiarily, it imposes no such requirements
     * on fwrite; it only requires fread to be broken.
     */
    if ((resid = count * size) == 0)
        return (0);
    if (fp->_r < 0)
        fp->_r = 0;
    total = resid;
    p = buf;
#if 1  /* BIONIC: optimize unbuffered reads */
    if (fp->_flags & __SNBF && fp->_ur == 0)
    {
        /* the following comes mainly from __srefill(), with slight
         * modifications
         */
        /* make sure stdio is set up */
        if (!__sdidinit)
            __sinit();
        fp->_r = 0;     /* largely a convenience for callers */
        /* SysV does not make this test; take it out for compatibility */
        if (fp->_flags & __SEOF)
            return (EOF);
        /* if not already reading, have to be reading and writing */
        if ((fp->_flags & __SRD) == 0) {
            if ((fp->_flags & __SRW) == 0) {
                errno = EBADF;
                fp->_flags |= __SERR;
                return (EOF);
            }
            /* switch to reading */
            if (fp->_flags & __SWR) {
                if (__sflush(fp))
                    return (EOF);
                fp->_flags &= ~__SWR;
                fp->_w = 0;
                fp->_lbfsize = 0;
            }
            fp->_flags |= __SRD;
        } else {
            /*
             * We were reading.  If there is an ungetc buffer,
             * we must have been reading from that.  Drop it,
             * restoring the previous buffer (if any).  If there
             * is anything in that buffer, return.
             */
            if (HASUB(fp)) {
                FREEUB(fp);
            }
        }
        /*
         * Before reading from a line buffered or unbuffered file,
         * flush all line buffered output files, per the ANSI C
         * standard.
         */
        if (fp->_flags & (__SLBF|__SNBF))
            (void) _fwalk(lflush);
        while (resid > 0) {
            int   len = (*fp->_read)(fp->_cookie, p, resid );
            fp->_flags &= ~__SMOD;
            if (len <= 0) {
                if (len == 0)
                    fp->_flags |= __SEOF;
                else {
                    fp->_flags |= __SERR;
                }
                return ((total - resid) / size);
            }
            p     += len;
            resid -= len;
        }
        return (count);
    }
    else
#endif
    {
        while (resid > (size_t)(r = fp->_r)) {
            (void)memcpy((void *)p, (void *)fp->_p, (size_t)r);
            fp->_p += r;
            /* fp->_r = 0 ... done in __srefill */
            p += r;
            resid -= r;
            if (__srefill(fp)) {
                /* no more input: return partial result */
                return ((total - resid) / size);
            }
        }
    }
    (void)memcpy((void *)p, (void *)fp->_p, resid);
    fp->_r -= resid;
    fp->_p += resid;
    return (count);
}

#undef ferror
int
ferror(FILE *fp)
{
	return (__sferror(fp));
}

size_t
fwrite(const void *buf, size_t size, size_t count, FILE *fp)
{
	size_t n;
	struct __suio uio;
	struct __siov iov;
	int ret;
	iov.iov_base = (void *)buf;
	uio.uio_resid = iov.iov_len = n = count * size;
	uio.uio_iov = &iov;
	uio.uio_iovcnt = 1;
	/*
	 * The usual case is success (__sfvwrite returns 0);
	 * skip the divide if this happens, since divides are
	 * generally slow and since this occurs whenever size==0.
	 */
	FLOCKFILE(fp);
	ret = __sfvwrite(fp, &uio);
	FUNLOCKFILE(fp);
	if (ret == 0)
		return (count);
	return ((n - uio.uio_resid) / size);
}

/*
 * ftello: return current offset.
 */
off_t
ftello(FILE *fp)
{
	fpos_t pos;
	if (fp->_seek == NULL) {
		errno = ESPIPE;			/* historic practice */
		pos = -1;
                goto out;
	}
	/*
	 * Find offset of underlying I/O object, then
	 * adjust for buffered bytes.
	 */
        FLOCKFILE(fp);
	__sflush(fp);		/* may adjust seek offset on append stream */
	if (fp->_flags & __SOFF)
		pos = fp->_offset;
	else {
		pos = (*fp->_seek)(fp->_cookie, (fpos_t)0, SEEK_CUR);
		if (pos == -1)
			goto out;
	}
	if (fp->_flags & __SRD) {
		/*
		 * Reading.  Any unread characters (including
		 * those from ungetc) cause the position to be
		 * smaller than that in the underlying object.
		 */
		pos -= fp->_r;
		if (HASUB(fp))
			pos -= fp->_ur;
	} else if (fp->_flags & __SWR && fp->_p != NULL) {
		/*
		 * Writing.  Any buffered characters cause the
		 * position to be greater than that in the
		 * underlying object.
		 */
		pos += fp->_p - fp->_bf._base;
	}
out:	FUNLOCKFILE(fp);
	return (pos);
}
/*
 * ftell() returns a long and sizeof(off_t) != sizeof(long) on all arches
 */
#if defined(__alpha__) && defined(__indr_reference)
__indr_reference(ftello, ftell);
#else
long
ftell(FILE *fp)
{
	long pos;
	pos = (long)ftello(fp);
	return(pos);
}


/* Flush a single file, or (if fp is NULL) all files.  */
int
fflush(FILE *fp)
{
	int r;
	if (fp == NULL)
		return (_fwalk(__sflush_locked));
	FLOCKFILE(fp);
	if ((fp->_flags & (__SWR | __SRW)) == 0) {
		errno = EBADF;
		r = EOF;
	} else
		r = __sflush(fp);
	FUNLOCKFILE(fp);
	return (r);
}
int
__sflush(FILE *fp)
{
	unsigned char *p;
	int n, t;
	t = fp->_flags;
	if ((t & __SWR) == 0)
		return (0);
	if ((p = fp->_bf._base) == NULL)
		return (0);
	n = fp->_p - p;		/* write this much */
	/*
	 * Set these immediately to avoid problems with longjmp and to allow
	 * exchange buffering (via setvbuf) in user write function.
	 */
	fp->_p = p;
	fp->_w = t & (__SLBF|__SNBF) ? 0 : fp->_bf._size;
	for (; n > 0; n -= t, p += t) {
		t = (*fp->_write)(fp->_cookie, (char *)p, n);
		if (t <= 0) {
			fp->_flags |= __SERR;
			return (EOF);
		}
	}
	return (0);
}
int
__sflush_locked(FILE *fp)
{
	int r;
	FLOCKFILE(fp);
	r = __sflush(fp);
	FUNLOCKFILE(fp);
	return (r);
}


int
fclose(FILE *fp)
{
	int r;
	if (fp->_flags == 0) {	/* not open! */
		errno = EBADF;
		return (EOF);
	}
	FLOCKFILE(fp);
	WCIO_FREE(fp);
	r = fp->_flags & __SWR ? __sflush(fp) : 0;
	if (fp->_close != NULL && (*fp->_close)(fp->_cookie) < 0)
		r = EOF;
	if (fp->_flags & __SMBF)
		free((char *)fp->_bf._base);
	if (HASUB(fp))
		FREEUB(fp);
	if (HASLB(fp))
		FREELB(fp);
	fp->_r = fp->_w = 0;	/* Mess up if reaccessed. */
	fp->_flags = 0;		/* Release this FILE for reuse. */
	FUNLOCKFILE(fp);
	return (r);
}
#endif

