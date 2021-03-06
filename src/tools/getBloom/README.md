## getBloom

`getBloom` retreives the bloom filter from the given block or blocks. Alternatively, you may also retrieve the bloom filters from the underlying transaction receipts that make up the block-level bloom. This tool is useful to debug QuickBlocks [monitors](../../monitors) among other things.

#### Usage

`Usage:`    getBloom [-r|-e|-a|-s|-p|-v|-h] &lt;block&gt; [block...]  
`Purpose:`  Returns bloom filter(s) from running node (the default) or as EAB from local cache.
             
`Where:`  

| Short Cut | Option | Description |
| -------: | :------- | :------- |
|  | block_list | a space-separated list of one or more blocks for which to retrieve blooms |
| -r | --raw | pull the bloom filter directly from the running node (the default) |
| -e | --eab | pull the enhanced adaptive bloom filters (EAB) from the local cache |
| -a | --asbits | display the bloom filter as bits instead of hex |
| -s | --asbars | display the bloom filter as bar charts instead of hex |
| -p | --receipt | display blooms from the transaction receipts as opposed to block (--raw only) |
| -v | --verbose | set verbose level. Either -v, --verbose or -v:n where 'n' is level |
| -h | --help | display this help screen |

`Notes:`

- `block_list` is a space-separated list of values, a start-end range, a `special`, or any combination.
- This tool retrieves information from the local node or the ${FALLBACK} node, if configured (see documentation).
- `special` blocks are detailed under `whenBlock --list`.

#### Other Options

All **quickBlocks** command-line tools support the following commands (although in some case, they have no meaning):

    Command     |     Description
    -----------------------------------------------------------------------------
    --version   |   display the current version of the tool
    --nocolor   |   turn off colored display
    --wei       |   specify value in wei (the default)
    --ether     |   specify value in ether
    --dollars   |   specify value in US dollars
    --file:fn   |   specify multiple sets of command line options in a file.

<small>*For the `--file:fn` option, place a series of valid command lines in a file and use the above options. In some cases, this option may significantly improve performance. A semi-colon at the start of a line makes that line a comment.*</small>

**Powered by QuickBlocks<sup>&reg;</sup>**


