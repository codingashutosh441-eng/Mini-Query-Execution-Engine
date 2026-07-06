import AceEditor from "react-ace";

import "ace-builds/src-noconflict/mode-sql";
import "ace-builds/src-noconflict/theme-github";

export default function SqlEditor({
  query,
  setQuery,
}) {
  return (
    <AceEditor
      mode="sql"
      theme="github"
      value={query}
      onChange={setQuery}
      width="100%"
      height="250px"
      fontSize={14}
      setOptions={{
        showLineNumbers: true,
      }}
    />
  );
}